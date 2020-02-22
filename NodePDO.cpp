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
    //discover the PDO number according the cob_id and the PDO type
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
            //if non of the above, something is wrong
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
    
    //store payload configuration
    _payloadStruct = payload;
    //calculate useful PDO size in bytes
    _size = 0;
    //look at PDO configuration to get the payload size
    for(int8_t i = 0; i < _payloadStruct.size(); i++)
    {
        _size += _payloadStruct[i].getDataTypeSize();
    }
    
    //size must be from 1 to 8 bytes
    if(_size > 8 || _size < 1)
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
    
    //prints if is receive or transmit
    ret << (_pdoType == PDOType::RX ? "rx" : "tx")
    //and its number
        << "PDO" << static_cast<int16_t>(_PDOnumber)
    //and its address
        << std::setw(7) << TO_HEX(_address)
        << std::endl;
    
    //and each one of its parts
    for(auto p: _payloadStruct)
    {
        ret << p.toString().str() << "\n";
    }
    
    return ret.str();
}

const std::vector<PDOpart>& NodePDO::getPayloadMap() const
{
    return _payloadStruct;
}

std::vector<int> NodePDO::valuesFromPayload(const CANframe& frame) const
{
    //there's no need of convert from payload to values in a transmit PDO
    if(_pdoType == PDOType::RX)
    {
        throw std::invalid_argument("This is NOT a txPDO! This is a rxPDO! (NodePDO.cpp:" + std::to_string(__LINE__) + ")\n");
    }
    
    //the return to be readable vector of values
    std::vector<int> ret;
    
    //will store the value (and its partial values)
    int currentValue;
    //the index of the current frame
    int frameIndex = 0;
    //counter for the bytes of a PDOpart
    int byteIndex = 0;
    
    //for each part of this PDO
    for(auto pdoPart : _payloadStruct)
    {
        //reset currentValue
        currentValue = 0;
        
        //walking through the PDOpart
        for(byteIndex = 0; byteIndex < pdoPart.getDataTypeSize(); byteIndex++)
        {
            //each value is store in Les Significant BYTE first fashion,
            //so each byte is left shift for 8*byteIndex bit positions and then is summed up to th currentValue
            currentValue = currentValue + (frame.data[frameIndex++] << (8 * byteIndex));
        }
        
        //storing in the return vector
        ret.push_back(currentValue);
    }
    
    //return the stuff
    return ret;
}

const PDOpart &NodePDO::at(int index) const
{
    return _payloadStruct.at(index);
}
