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

PDOpart::PDOpart(int8_t index, const std::string &description, CANopenDataType dataType,
                 DictionaryEntry& dictionaryEntry) : _index(index), _description(description), _dataType(dataType), _dictionaryEntry(dictionaryEntry)
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

PDOpart::PDOpart(): _index(0), _description(""), _dataType(CANopenDataType::INT8), _dataTypeName(""), _dataTypeSize(0), _dictionaryEntry(
        const_cast<DictionaryEntry &>(dummy))
{
}

int& PDOpart::getDictionaryEntryValueRef()
{
    if(_dictionaryEntry == dummy)
    {
        throw std::runtime_error("This PDOpart has no _dictonaryEntry yet! (PDOpart.cpp:" + std::to_string(__LINE__) + ")\n");
    }
    
    return std::get<0>(_dictionaryEntry);
}

PDOpart::PDOpart(const PDOpart &other): _index(other._index), _description(other._description), _dataType(other._dataType), _dataTypeName(other._dataTypeName), _dataTypeSize(other._dataTypeSize), _dictionaryEntry(other._dictionaryEntry)
{
}

PDOpart &PDOpart::operator=(const PDOpart& other)
{
    /*if (this != &other) { // self-assignment check expected
        this->dummy
        }
        std::copy(other.mArray, other.mArray + other.size, mArray);
    }*/
    return *this;
}
