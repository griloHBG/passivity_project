//
// Created by grilo on 11/02/2020.
//

#include "PDOpart.h"

std::stringstream PDOpart::toString() const
{
    // it's straightforward
    std::stringstream ss;
    ss  << std::setw(2)  << static_cast<uint16_t>(_index)
        << std::setw(20) << _description
        << std::setw(7)  << _dataTypeName;
    
    return ss;
}

PDOpart::PDOpart(int8_t index, const std::string &description, CANopenDataType dataType) : _index(index), _description(description), _dataType(dataType)
{
    //setting type name and size
    switch(dataType)
    {
        case CANopenDataType::INT8:
            _dataTypeName = "INT8";
            _dataTypeSize = 1;
            break;
        case CANopenDataType::INT16:
            _dataTypeName = "INT16";
            _dataTypeSize = 2;
            break;
        case CANopenDataType::INT32:
            _dataTypeName = "INT32";
            _dataTypeSize = 4;
            break;
        case CANopenDataType::UINT8:
            _dataTypeName = "UINT8";
            _dataTypeSize = 1;
            break;
        case CANopenDataType::UINT16:
            _dataTypeName = "UINT16";
            _dataTypeSize = 2;
            break;
        case CANopenDataType::UINT32:
            _dataTypeName = "UINT32";
            _dataTypeSize = 4;
            break;
        case CANopenDataType::STRING:
            _dataTypeName = "STRING";
            _dataTypeSize = 8;
            break;
        case CANopenDataType::UINT64:
            _dataTypeName = "UINT64";
            _dataTypeSize = 8;
            break;
    }
}

int8_t PDOpart::getIndex() const
{
    return _index;
}

std::string PDOpart::getDescription() const
{
    return _description;
}

CANopenDataType PDOpart::getDataType() const
{
    return _dataType;
}

uint8_t PDOpart::getDataTypeSize() const
{
    return _dataTypeSize;
}

const std::string &PDOpart::getDataTypeName() const
{
    return _dataTypeName;
}
