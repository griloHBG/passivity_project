//
// Created by grilo on 25/02/2020.
//

#include "CANopenUtils.h"
/*
template <typename T>
constexpr auto type_name()
{
    std::string_view name, prefix, suffix;
#ifdef __clang__
    name = __PRETTY_FUNCTION__;
    prefix = "auto type_name() [T = ";
    suffix = "]";
#elif defined(__GNUC__)
    name = __PRETTY_FUNCTION__;
    prefix = "constexpr auto type_name() [with T = ";
    suffix = "]";
#elif defined(_MSC_VER)
    name = __FUNCSIG__;
    prefix = "auto __cdecl type_name<";
    suffix = ">(void)";
#endif
    name.remove_prefix(prefix.size());
    name.remove_suffix(suffix.size());
    return name;
}
*/

std::string printCANframe(const can_frame& frame)
{
    std::stringstream ss;
    
    ss << std::setw(6) << TO_HEX(static_cast<uint16_t>(frame.can_id)) << " #";
    
    for(auto d: frame.data)
    {
        ss << std::setw(6) << TO_HEX(static_cast<uint16_t>(d));
    }
    
    ss << std::endl;
    
    return ss.str();
}

CANframe createSDO(const uint8_t address, const bool &doWrite, const IndexSubindex &indexSubindex,
                   const CANopenDataType &dataType, const int &value)
{
    // flag for error at data
    bool dataError = false;
    
    // amount of data inn bytes that will not be used in this SDO message (related to n value of the SDO message)
    uint8_t invalidData = 4;
    
    // data type analysis
    switch(dataType)
    {
        case CANopenDataType::INT8:
        case CANopenDataType::UINT8:
            invalidData = 4 - 1;
            dataError = ((value >> 8) != 0);
            break;
        case CANopenDataType::INT16:
        case CANopenDataType::UINT16:
            invalidData = 4 - 2;
            dataError = ((value >> 8*2) != 0);
            break;
        case CANopenDataType::INT32:
        case CANopenDataType::UINT32:
            invalidData = 4 - 4;
            break;
        case CANopenDataType::UINT64:
        case CANopenDataType::STRING:
            throw std::invalid_argument("Not implemented for type UINT64 nor STRING! (CANopenCommunication.cpp:" + std::to_string(__LINE__) + ")\n");
            break;
        default:
            //if read, do nothing
            if(doWrite)
            {
                dataError = true;
            }
    }
    
    // if error, print so
    if(dataError)
    {
        throw std::invalid_argument("maximum type expected is 4 bytes! (CANopenCommunication.cpp:" + std::to_string(__LINE__) + ")\n");
    }
    
    // message that will return
    CANframe SDOmessage;
    
    // amount of bytes used
    SDOmessage.can_dlc = 8;
    
    // SDO message ID
    SDOmessage.can_id = 0x600 + address;
    
    // alias for the first byte of data (command byte for SDO message)
    uint8_t& commandByte = SDOmessage.data[0];
    
    // set object index
    SDOmessage.data[1] = (indexSubindex >> 8) & 0xFF;
    SDOmessage.data[2] = indexSubindex >> 16;
    
    // set object subindex
    SDOmessage.data[3] = indexSubindex & 0xFF;
    
    // data for payload of SDO message
    std::array<uint8_t, 4> data{};
    
    // if it is and SDO download message (that writes in the node that receives the message - the server)
    if (doWrite) //SDO download
    {
        //client command specifier (ccs)
        uint8_t ccs = 1;
        //message is not segmented!
        uint8_t expedited = 1;
        //size IS indicated
        uint8_t size = 1;
        
        // commandByte is made of (from MSB to LSB): ccs (3bits), 1 reserved bit, n (2bits), e (1bit), s (1bit)
        commandByte = (ccs << 5) | (invalidData << 2) | (expedited << 1) | size;
        
        // set data (for payload)
        for(int i = 0; i < (4 - invalidData); i++)
        {
            data.at(i) = (value >> 8*i) & (0xFF);
        }
    }
    else // read: SDO upload
    {
        //client command specifier (ccs)
        uint8_t ccs = 2;
        
        // commandByte is made of (from MSB to LSB): ccs (3bits), 1 reserved bit, n (2bits), e (1bit), s (1bit)
        // online ccs is needed for upload (read)
        commandByte = (ccs << 5);
    }
    
    // the first byte at data
    SDOmessage.data[0] = commandByte;
    
    // set payload
    for(int i = 0; i < 4; i++)
    {
        SDOmessage.data[i+4] = data.at(i);
    }
    
    return SDOmessage;
}

unsigned int createCOBID(PDOType pdoType, uint8_t pdoNumber, uint8_t nodeAddress)
{
    /*
    ///TXPDO1 message
    SEND_PDO1   = 0x03,
    ///RXPDO1 message
    RECV_PDO1   = 0x04,
    ///TXPDO2 message
    SEND_PDO2   = 0x05,
    ///RXPDO2 message
    RECV_PDO2   = 0x06,
    ///TXPDO3 message
    SEND_PDO3   = 0x07,
    ///RXPDO3 message
    RECV_PDO3   = 0x08,
    ///TXPDO4 message
    SEND_PDO4   = 0x09,
    ///RXPDO4 message
    RECV_PDO4   = 0x0A,
     */
    
    uint8_t pdoFuncCode = 0x03 + static_cast<uint8_t>(pdoType) + 2*(pdoNumber-1);
    
    return (pdoFuncCode << 7) | nodeAddress;
    
}
