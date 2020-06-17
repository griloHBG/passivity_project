//
// Created by grilo on 02/02/2020.
//

#ifndef PASSIVITY_PROJECT_PDOMESSAGE_H
#define PASSIVITY_PROJECT_PDOMESSAGE_H


#include <cstdint>
#include <map>
#include <vector>
#include <linux/can.h>
#include <iostream>
#include "PDOpart.h"
#include "CANopenUtils.h"

class PDOmessage
{
public:
    /// Ctor for this PDOmessage class
    /// \param pdoType PDO's type: receive (RX) or transmit (TX). From the point of view of the real CANopen node.
    /// \param pdoNumber PDO's number (1, 2, 3 or 4)
    /// \param payload the configuration of the current PDO using vector of PDO part.
    //TODO: there's no need to enter with cob_id! function is predefined and the address if from the node!
    PDOmessage(const PDOType pdoType, uint8_t pdoNumber, uint8_t nodeAddress,
               const std::vector<PDOpart> &payload);
    
    PDOmessage();
    
    /// Getter to payload mapping. This is the actual PDO configuration
    /// \return vector with this PDO's configuration
    const std::vector<PDOpart>& getPayloadMap() const;
    
    /// Getter to PDO type (RX, TX)
    /// \return this PDO type
    PDOType getPdoType() const;
    
    /// Getter to PDO number
    /// \return this PDO number
    unsigned char getNumber() const;
    
    /// Getter to address
    /// \return this PDO address
    unsigned int getCOBID() const;
    
    /// Converts this PDO to a readable string. Usefull to assure that thaPDO was correctly configured
    /// \return String explaining this PDOmessage configuration
    std::string toString() const;
    
    /// a main public PDOmessage's function used to convert a CanFrame's payload to a readable vector with values, accordingly to the PDO message setup
    /// \param frame CANframe to be converted
    /// \return a vector with values according the PDO configuration
    std::vector<int> valuesFromPayload(const CANframe &frame) const;
    
    /*
    /// a main public PDOmessage's function used to convert others to a PDO paylod formating (calls _payloadFromValues(first))
    /// \tparam T1 type of the argument
    /// \param first argument value
    /// \return a vector containing the right PDO payload format
    template<typename T1>
    std::vector<uint8_t> payloadFromValues(T1 first);
    */
    /// a main public PDOmessage's function used to convert others to a PDO paylod formating (calls _payloadFromValues(first,... others))
    /// \tparam T1 type of the first argument
    /// \tparam Tn types of the others arguments
    /// \param first first argument value
    /// \param others others argument others
    /// \return a vector containing the right PDO payload format
    template<typename... Tn>
    std::vector<uint8_t> payloadFromValues(Tn... allValues);
    
    /// similar to std::vector's at function
    /// \param index index of the wanted PDO part
    /// \return the PDO part wanted
    const PDOpart& at(int index) const;
    
    uint8_t getSize();
    
private:
    
    ///PDO type of this object (transmit or receive)
    PDOType _pdoType;
    
    ///PDO's number (PDO1, PDO2, PDO3 or PDO4)
    unsigned char _PDOnumber;
    
    ///PDO's address
    uint16_t _cobid;
    
    ///Stores how the PDO is configured (index position, size, description...)
    std::vector<PDOpart> _payloadStruct;
    
    ///Actual PDO size (less or equal than 8 bytes)
    uint8_t _size;
    
    /// a main private PDOmessage's function used to convert others to a PDO paylod formating
    /// \tparam T1 type of the first argument
    /// \tparam Tn types of the others arguments
    /// \param currentIndexToUse stores which PDO payload position should be used
    /// \param first first argument value
    /// \param others others argument others
    /// \return a vector containing the right PDO payload format
    template<typename T1, typename... Tn>
    std::vector<uint8_t> _payloadFromValues(uint8_t& currentIndexToUse, T1 first, Tn... others);
    
    /// a main private PDOmessage's function used to convert others to a PDO paylod formating
    /// \tparam T1 type of the argument
    /// \param currentIndexToUse
    /// \param first argument value
    /// \return a vector containing the right PDO payload format
    template<typename T1>
    std::vector<uint8_t> _payloadFromValues(uint8_t& currentIndexToUse, T1 first);
};
/*
template<typename T1>
std::vector<uint8_t> PDOmessage::payloadFromValues(T1 first)
{
    //there is no need to convert from value to payload on a transmit PDO
    if(_pdoType == PDOType::TX)
    {
        throw std::invalid_argument("This is NOT a rxPDO! This is a txPDO! (PDOmessage.h:" + std::to_string(__LINE__) + ")\n");
    }
    
    //of course stuff will start on index 0;
    uint8_t index = 0;
    
    //run the respective main function
    auto ret = _payloadFromValues(index, first);
    
    //sanity check on size of returned payload
    //TODO optimize it out according to debug compiling profile?
    if(ret.size() < _size)
    {
        //print stuff
        throw std::invalid_argument("Only " + std::to_string(ret.size()) + " out of " + std::to_string(_size) + " bytes were filled! Not good! (PDOmessage.h:" + std::to_string(__LINE__) + ")\n");
    }
    
    //fill with zeros the unused part of PDO's payload
    for(int i = ret.size(); i < 8; i++)
    {
        ret.push_back(0);
    }
    
    return ret;
}
*/
template<typename... Tn>
std::vector<uint8_t> PDOmessage::payloadFromValues(Tn... allValues)
{
    //there is no need to convert from value to payload on a transmit PDO
    if(_pdoType == PDOType::TX)
    {
        throw std::invalid_argument("This is NOT a rxPDO! This is a txPDO! (PDOmessage.h:" + std::to_string(__LINE__) + ")\n");
    }
    
    //of course stuff will start on index 0;
    uint8_t index = 0;
    
    //run the respective main function
    auto ret = _payloadFromValues(index, allValues...);
    
    //sanity check on size of returned payload
    //TODO optimize it out according to debug compiling profile?
    if(ret.size() < _size)
    {
        //print stuff
        throw std::invalid_argument("Only " + std::to_string(ret.size()) + " out of " + std::to_string(_size) + " bytes were filled! Not good! (PDOmessage.h:" + std::to_string(__LINE__) + ")\n");
    }
    
    //fill with zeros the unused part of PDO's payload
    for(int i = ret.size(); i < 8; i++)
    {
        ret.push_back(0);
    }
    
    return ret;
}

template<typename T1, typename... Tn>
std::vector<uint8_t> PDOmessage::_payloadFromValues(uint8_t& currentIndexToUse, T1 first, Tn... others)
{
    //unpacking the argument "first"
    std::vector<uint8_t> ret = _payloadFromValues(currentIndexToUse, first);
    //recursion for "others"
    std::vector<uint8_t> other = _payloadFromValues(currentIndexToUse, others...);
    
    //MOVING (no copying!) everything to ret
    ret.insert(ret.end(), std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
    
    //and return
    return ret;
}

template<typename T1>
std::vector<uint8_t> PDOmessage::_payloadFromValues(uint8_t& currentIndexToUse, T1 first)
{
    //only a shortcut
    const uint8_t& index = currentIndexToUse;
    //erro flag for problems
    bool error = false;
    
    //if the expected size is less than 4 bytes (32 bits)
    if (_payloadStruct[index].getDataTypeSize() < 4)
    {
        //evaluate if the number is REALLY 4 bytes
        if (first >> (8 * _payloadStruct[index].getDataTypeSize()) != 0)
        {
            //if not, set the error flag
            error = true;
        }
    }
    
    //if there's and error
    if(error)
    {
        //print it
        std::stringstream ss;
        ss << "Value " << first << " (" << TO_HEX(first) << ") " << " doesn't fit data ("
           << _payloadStruct[index].getDescription() << ") type expected (" << _payloadStruct[index].getDataTypeName()
           << " - " << static_cast<int16_t>(_payloadStruct[index].getDataTypeSize()) << " byte(s) - )!\n";
    
        throw std::invalid_argument(ss.str() + "(PDOmessage.h:" + std::to_string(__LINE__) + ")\n");
    }
    
    //return vectir (in RVO -return value optimization - we trust!)
    std::vector<uint8_t> ret;
    
    //store the first byte of "first"
    ret.push_back(first & 0xFF);
    
    //for the remaining bytes of "first" (if any)
    for(uint8_t i = 1; i < _payloadStruct[index].getDataTypeSize(); i++)
    {
        //right shift of 1 byte
        first >>= 8;
        //store the next byte
        ret.push_back(first & 0xFF);
    }
    
    //next time, let's use the next PDOpart element of this PDO
    currentIndexToUse += 1;
    
    return ret;
}


#endif //PASSIVITY_PROJECT_PDOMESSAGE_H
