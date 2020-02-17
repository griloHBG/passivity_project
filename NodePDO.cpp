//
// Created by grilo on 02/02/2020.
//

#include <cassert>
#include <sstream>
#include <iomanip>
#include "NodePDO.h"
#include "utils.h"

NodePDO::NodePDO(const PDOType pdoType, uint16_t cob_id, const std::vector<PDOpart> &payload) : _pdoType(pdoType), _address(cob_id)
{
    if(_pdoType == PDOType::TX)
    {
        if(0x181 <= cob_id && cob_id <= 0x01FF)
        {
            //txPDO1
            _PDOnumber = 1;
        }
        else if(0x281 <= cob_id && cob_id <= 0x02FF)
        {
            //txPDO2
            _PDOnumber = 2;
        }
        else if(0x381 <= cob_id && cob_id <= 0x03FF)
        {
            //txPDO3
            _PDOnumber = 3;
        }
        else if(0x481 <= cob_id && cob_id <= 0x04FF)
        {
            //txPDO4
            _PDOnumber = 4;
        }
        else //fail!
        {
            std::stringstream errorss;
            errorss << "COB-ID " << TO_HEX(cob_id) << " for txPDO not valid!\n"
                                                      "Valid ranges: [0x0181, 0x01FF] for txPDO1 or "
                                                      "[0x0281, 0x02FF] for txPDO2 or "
                                                      "[0x0381, 0x03FF] for txPDO3 or "
                                                      "[0x0481, 0x04FF] for txPDO4.\n";
            throw std::invalid_argument(errorss.str());
        }
    }
    else //this->_pdoType == PDOType::RX
    {
        if(0x201 <= cob_id && cob_id <= 0x027F)
        {
            //rxPDO1
            _PDOnumber = 1;
        }
        else if(0x301 <= cob_id && cob_id <= 0x037F)
        {
            //rxPDO2
            _PDOnumber = 2;
        }
        else if(0x401 <= cob_id && cob_id <= 0x047F)
        {
            //rxPDO3
            _PDOnumber = 3;
        }
        else if(0x501 <= cob_id && cob_id <= 0x057F)
        {
            //rxPDO4
            _PDOnumber = 4;
        }
        else //fail!
        {
            std::stringstream errorss;
            errorss << "COB-ID " << TO_HEX(cob_id) << " for txPDO not valid!\n"
                                                      "Valid ranges: [0x0201, 0x027F] for txPDO1 or "
                                                      "[0x0301, 0x037F] for txPDO2 or "
                                                      "[0x0401, 0x047F] for txPDO3 or "
                                                      "[0x0501, 0x057F] for txPDO4.\n";
            throw std::invalid_argument(errorss.str());
        }
    }
    
    _payloadStruct = payload;
    _size = 0;
    for(int8_t i = 0; i < _payloadStruct.size(); i++)
    {
        _size += _payloadStruct[i].getDataTypeSize();
    }
    
    if(_size > 8)
    {
        throw std::invalid_argument("Received Payload Struct sums more than 8 bytes (it is summing " + std::to_string(_size) + ")!");
    }
    
}

PDOType NodePDO::getPdoType() const
{
    return _pdoType;
}

unsigned char NodePDO::getNumber() const
{
    return _PDOnumber;
}

unsigned int NodePDO::getAddress() const
{
    return _address;
}


std::string NodePDO::toString() const
{
    std::stringstream ret;
    
    ret << (_pdoType == PDOType::RX ? "rx" : "tx") << "PDO" << static_cast<int16_t>(_PDOnumber) << std::setw(7) << TO_HEX(_address) << std::endl;
    
    uint8_t last = -1;
    uint8_t currentSize = 0;
    
    for(auto p: _payloadStruct)
    {
        ret << p.toString().str() << "\n";
    }
    
    //ret << std::setw(2) << static_cast<uint16_t>(last) << std::setw(20) << _payloadMap.at(last) << " size " << static_cast<uint16_t>(currentSize) << " bytes" << std::endl;
    
    
    return ret.str();
}

const std::vector<PDOpart>& NodePDO::getPayloadMap() const
{
    return _payloadStruct;
}

/*
const std::array<signed char, 8> &NodePDO::getPayload() const
{
    return _payload;
}
 */

std::vector<int> NodePDO::valuesFromPayload(const CANframe& frame) const
{
    if(_pdoType == PDOType::RX)
    {
        throw std::invalid_argument("This is NOT a txPDO! This is a rxPDO! (NodePDO.cpp:" + std::to_string(__LINE__) + ")\n");
    }
    
    std::vector<int> ret;
    
    int lastIndex = -1;
    int aux;
    int frameIndex = 0;
    
    for(auto pdoPart : _payloadStruct)
    {
        aux = 0;
        
        for(int i = 0; i < pdoPart.getDataTypeSize(); i++)
        {
            aux = aux + (frame.data[frameIndex++] << (8*i));
        }
        
        ret.push_back(aux);
    }
    return ret;
}

const PDOpart &NodePDO::at(int index) const
{
    return _payloadStruct.at(index);
}
