//
// Created by grilo on 02/02/2020.
//

#ifndef PASSIVITY_PROJECT_NODEPDO_H
#define PASSIVITY_PROJECT_NODEPDO_H


#include <cstdint>
#include <map>
#include <vector>
#include <linux/can.h>
#include <iostream>
#include "CANopenTypes.h"
#include "PDOpart.h"

#define TO_HEX(var) std::hex << "0x" << std::uppercase << var << std::dec << std::nouppercase
#define TEST_FOR_INT_TYPE(var, type) static_cast<type>(var) == var ? true : false

class NodePDO
{
public:
    NodePDO(PDOType pdoType, uint16_t cob_id, const std::vector<PDOpart> &payload);
    
    const std::vector<PDOpart>& getPayloadMap() const;
    
    //const std::array<int8_t, 8> &getPayload() const;
    
    PDOType getPdoType() const;
    
    unsigned char getNumber() const;
    
    unsigned int getAddress() const;
    
    std::string toString() const;
    
    std::vector<int> valuesFromPayload(const can_frame &frame) const;
    
    template<typename T1>
    std::vector<uint8_t> payloadFromValues(T1 first);
    
    template<typename T1, typename... Tn>
    std::vector<uint8_t> payloadFromValues(T1 first, Tn... values);
    
    const PDOpart& at(int index) const;
    
private:
    uint16_t _address;
    std::vector<PDOpart> _payloadStruct; //position on PDO and its explanation
    //std::array<int8_t , 8> _payload;
    ///PDO type of this object (transmit or receive)
    PDOType _pdoType;
    unsigned char _PDOnumber;
    uint8_t _size;
    
    template<typename T1, typename... Tn>
    std::vector<uint8_t> _payloadFromValues(uint8_t& currentIndexToUse, T1 first, Tn... values);
    
    template<typename T1>
    std::vector<uint8_t> _payloadFromValues(uint8_t& currentIndexToUse, T1 first);
};

///
/// \tparam Tn template list of other arguments
/// \param first the first argument (an int)
/// \param values other arguments (packed parameters)
/// \return a vector of uint8_t that should be used as the payload of a CANopen PDO message
template<typename T1>
std::vector<uint8_t> NodePDO::payloadFromValues(T1 first)
{
    
    uint8_t index = 0;
    
    return _payloadFromValues(index, first);
}


///
/// \tparam Tn template list of other arguments
/// \param first the first argument (an int)
/// \param values other arguments (packed parameters)
/// \return a vector of uint8_t that should be used as the payload of a CANopen PDO message
template<typename T1, typename... Tn>
std::vector<uint8_t> NodePDO::payloadFromValues(T1 first, Tn... values)
{
    if(_pdoType == PDOType::TX)
    {
        throw std::invalid_argument("This is NOT a rxPDO! This is a txPDO!\n");
    }
    
    uint8_t index = 0;
    
    auto ret = _payloadFromValues(index, first, values...);
    
    if(ret.size() < _size)
    {
        throw std::invalid_argument("Only " + std::to_string(ret.size()) + " out of " + std::to_string(_size) + " bytes were filled! Not good!");
    }
    
    for(int i = ret.size(); i < 8; i++)
    {
        ret.push_back(0);
    }
    
    return ret;
}

///
/// \tparam Tn template list of other arguments
/// \param first the first argument (an int)
/// \param values other arguments (packed parameters)
/// \return a vector of uint8_t that should be used as the payload of a CANopen PDO message
template<typename T1, typename... Tn>
std::vector<uint8_t> NodePDO::_payloadFromValues(uint8_t& currentIndexToUse, T1 first, Tn... values)
{
    std::vector<uint8_t> ret = _payloadFromValues(currentIndexToUse, first);
    std::vector<uint8_t> other = _payloadFromValues(currentIndexToUse, values...);
    
    ret.insert(ret.end(), std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
    
    return ret;
}

template<typename T1>
std::vector<uint8_t> NodePDO::_payloadFromValues(uint8_t& currentIndexToUse, T1 first)
{
    uint8_t index = currentIndexToUse;
    bool error = false;
    
    if (_payloadStruct[index].getDataTypeSize() < 4)
    {
        if (first >> (8 * _payloadStruct[index].getDataTypeSize()) != 0)
        {
            error = true;
        }
    }
    
    if(error)
    {
        std::stringstream ss;
        ss << "Value " << first << " (" << TO_HEX(first) << ") " << " doesn't fit data ("
           << _payloadStruct[index].getDescription() << ") type expected (" << _payloadStruct[index].getDataTypeName()
           << " - " << static_cast<int16_t>(_payloadStruct[index].getDataTypeSize()) << " byte(s) - )!\n";
    
        throw std::invalid_argument(ss.str());
    }
    
    std::vector<uint8_t> ret;
    
    ret.push_back(first & 0xFF);
    
    for(uint8_t i = 1; i < _payloadStruct[index].getDataTypeSize(); i++)
    {
        first = first >> 8;
        ret.push_back(first & 0xFF);
    }
    
    currentIndexToUse += 1;
    
    return ret;
}


#endif //PASSIVITY_PROJECT_NODEPDO_H
