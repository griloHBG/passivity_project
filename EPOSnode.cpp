//
// Created by grilo on 25/02/2020.
//

#include <utility>
#include "EPOSnode.h"

void EPOSnode::updateSDOobject(const CANframe &frame) {
    //TODO value from payload for SDO
//    _objectDictionary[IndexSubindex(frame.data)] =
}

EPOSnode::EPOSnode(uint8_t address)
        : CANopenNode(address)
{
        ObjectsDictionary& dis = _dictionaryByIndexSubindex;
        dis[toIndexSubindex(0x2030, 0x00)] = {0, CANopenDataType::INT16,  toIndexSubindex(0x2030, 0x00), "CurrentMode Setting Value"};
        dis[toIndexSubindex(0x2062, 0x00)] = {0, CANopenDataType::INT32,  toIndexSubindex(0x2062, 0x00), "PositionMode Setting Value"};
        dis[toIndexSubindex(0x206B, 0x00)] = {0, CANopenDataType::INT32,  toIndexSubindex(0x206B, 0x00), "VelocityMode Setting Value"};
        dis[toIndexSubindex(0x6040, 0x00)] = {0, CANopenDataType::UINT16, toIndexSubindex(0x6040, 0x00), "ControlWord"};
        dis[toIndexSubindex(0x6041, 0x00)] = {0, CANopenDataType::UINT16, toIndexSubindex(0x6041, 0x00), "StatusWord"};
        dis[toIndexSubindex(0x6062, 0x00)] = {0, CANopenDataType::INT32,  toIndexSubindex(0x6062, 0x00), "Position Demand Value"};
        dis[toIndexSubindex(0x6064, 0x00)] = {0, CANopenDataType::INT32,  toIndexSubindex(0x6064, 0x00), "Position Actual Value"};
        dis[toIndexSubindex(0x606C, 0x00)] = {0, CANopenDataType::INT32,  toIndexSubindex(0x606C, 0x00), "Velocity Actual Value"};
        dis[toIndexSubindex(0x6078, 0x00)] = {0, CANopenDataType::INT32,  toIndexSubindex(0x6078, 0x00), "Current Actual Value"};
        dis[toIndexSubindex(0x607A, 0x00)] = {0, CANopenDataType::INT32,  toIndexSubindex(0x607A, 0x00), "Target Position"};
        dis[toIndexSubindex(0x60FF, 0x00)] = {0, CANopenDataType::INT32,  toIndexSubindex(0x60FF, 0x00), "TargetVelocity"};
    
    configurePDOs({PDOmessage(PDOType::TX, 0x181, {PDOpart(0, "Position Actual Value", CANopenDataType::INT32, dis[toIndexSubindex(0x6064, 0x00)]),
                                                   PDOpart(4, "Current Actual Value", CANopenDataType::INT16, dis[toIndexSubindex(0x6078, 0x00)]),
                                                   PDOpart(6, "StatusWord", CANopenDataType::UINT16, dis[toIndexSubindex(0x6041, 0x00)])}),
                        
                   PDOmessage(PDOType::TX, 0x281, {PDOpart(0, "Velocity Actual Value", CANopenDataType::INT32, dis[toIndexSubindex(0x606C, 0x00)])}),
                   
                   PDOmessage(PDOType::TX, 0x381, {PDOpart(0, "StatusWord", CANopenDataType::UINT16, dis[toIndexSubindex(0x6041, 0x00)]),
                                                   PDOpart(4, "Position Actual Value", CANopenDataType::INT32, dis[toIndexSubindex(0x6064, 0x00)])}),
                                                   
                   PDOmessage(PDOType::TX, 0x481, {PDOpart(0, "StatusWord", CANopenDataType::UINT16, dis[toIndexSubindex(0x6041, 0x00)]),
                                                   PDOpart(2, "Velocity Actual Value", CANopenDataType::INT32, dis[toIndexSubindex(0x606C, 0x00)])})},
                                                   
                  {PDOmessage(PDOType::RX, 0x201, {PDOpart(0, "PositionMode Setting Value", CANopenDataType::INT32, dis[toIndexSubindex(0x2062, 0x00)]),
                                                   PDOpart(4, "CurrentMode Setting Value", CANopenDataType::INT16, dis[toIndexSubindex(0x2030, 0x00)]),
                                                   PDOpart(6, "ControlWord", CANopenDataType::UINT16, dis[toIndexSubindex(0x6040, 0x00)])}),
                   
                   PDOmessage(PDOType::RX, 0x301, {PDOpart(0, "VelocityMode Setting Value", CANopenDataType::INT32, dis[toIndexSubindex(0x206B, 0x00)])}),
        
                   PDOmessage(PDOType::RX, 0x401, {PDOpart(0, "ControlWord", CANopenDataType::UINT16, dis[toIndexSubindex(0x6040, 0x00)]),
                                                   PDOpart(2, "Target Position", CANopenDataType::INT32, dis[toIndexSubindex(0x607A, 0x00)])}),
        
                   PDOmessage(PDOType::RX, 0x501, {PDOpart(0, "ControlWord", CANopenDataType::UINT16, dis[toIndexSubindex(0x6040, 0x00)]),
                                                   PDOpart(2, "TargetVelocity", CANopenDataType::INT32, dis[toIndexSubindex(0x60FF, 0x00)])}),
                  });
}

CANframe EPOSnode::setPositionActualValue(int32_t value)
{
    return createSDO(true, _dictionaryByIndexSubindex[toIndexSubindex(0x6064, 0x00)], value);
}
