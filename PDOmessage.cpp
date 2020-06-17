//
// Created by grilo on 02/02/2020.
//

#include <cassert>
#include <sstream>
#include <iomanip>
#include "PDOmessage.h"

PDOmessage::PDOmessage(const PDOType pdoType, uint8_t pdoNumber, uint8_t nodeAddress,
                       const std::vector<PDOpart> &payload) : _pdoType(pdoType), _PDOnumber(pdoNumber), _cobid(createCOBID(_pdoType, pdoNumber, nodeAddress)), _payloadStruct(payload)
{
    //calculate useful PDO size in bytes
    _size = 0;
    
    //look at PDO configuration to get the payload size
    for(uint8_t i = 0; i < _payloadStruct.size(); i++)
    {
        _size += _payloadStruct[i].getDataTypeSize();
    }
    
    //size must be from 1 to 8 bytes
    if(_size > 8 || _size < 1)
    {
        throw std::invalid_argument("Received Payload Struct sums more than 8 bytes (it is summing " + std::to_string(_size) + ")! (PDOmessage.cpp:" + std::to_string(__LINE__) + ")\n");
    }
    
}

PDOType PDOmessage::getPdoType() const
{
    return _pdoType;
}

unsigned char PDOmessage::getNumber() const
{
    return _PDOnumber;
}

unsigned int PDOmessage::getCOBID() const
{
    return _cobid;
}


std::string PDOmessage::toString() const
{
    std::stringstream ret;
    
    //prints if is receive or transmit
    ret << (_pdoType == PDOType::RX ? "rx" : "tx")
    //and its number
        << "PDO" << static_cast<int16_t>(_PDOnumber)
    //and its address
        << std::setw(7) << TO_HEX(_cobid)
        << std::endl;
    
    //and each one of its parts
    for(auto p: _payloadStruct)
    {
        ret << p.toString().str() << "\n";
    }
    
    return ret.str();
}

const std::vector<PDOpart>& PDOmessage::getPayloadMap() const
{
    return _payloadStruct;
}

std::vector<int> PDOmessage::valuesFromPayload(const CANframe& frame) const
{
    //there's no need of convert from payload to values in a transmit PDO
    if(_pdoType == PDOType::RX)
    {
        throw std::invalid_argument("This is NOT a txPDO! This is a rxPDO! (PDOmessage.cpp:" + std::to_string(__LINE__) + ")\n");
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
            //each value is store in Less Significant BYTE first fashion,
            //so each byte is left shift for 8*byteIndex bit positions and then is summed up to the currentValue
            currentValue = currentValue + (frame.data[frameIndex++] << (8u * byteIndex));
        }
        
        //storing in the return vector
        ret.push_back(currentValue);
    }
    
    //return the stuff
    return ret;
}

const PDOpart &PDOmessage::at(int index) const
{
    return _payloadStruct.at(index);
}

PDOmessage::PDOmessage() : _pdoType(PDOType::RX), _PDOnumber(0), _cobid(0), _payloadStruct({PDOpart()}), _size(0)
{

}

uint8_t PDOmessage::getSize()
{
    return _size;
}
