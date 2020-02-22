//
// Created by grilo on 03/02/2020.
//

#ifndef PASSIVITY_PROJECT_CANOPENTYPES_H
#define PASSIVITY_PROJECT_CANOPENTYPES_H

#include <sstream>
#include <iomanip>

typedef int Socket;
typedef struct sockaddr_can CANaddress;
typedef struct sockaddr Address;
typedef struct ifreq InterfRequest;
typedef struct can_frame CANframe;

const int INDEX = 0;
const int NAME = 1;
const int SIZE = 2;

///Function Code for each type of message
enum FUNC_CODE: uint8_t
{
    ///Network Management message
    NMT         = 0x00,
    ///Synchronization message
    SYNC        = 0x01,
    ///Emergency message
    EMCY        = 0x01,
    ///Time message
    TIME        = 0x02,
    
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
    RECV_PDO4   = 0x10,
    
    ///Send SDO message
    SEND_SDO    = 0x11,
    ///Receive SDO message
    RECV_SDO    = 0x12,
    
    ///Heartbeat message
    HEARTBEAT = 0x14,
};

///Each type of NMT command
enum NMTCOMMAND: uint8_t
{
    ///Operaational mode
    OPERATIONAL = 0x01,
    ///Stopped mode
    STOPPED = 0x02,
    ///Pre-operaational mode
    PREOPERATIONAL = 0x80,
    ///Reset Application
    RESET_APPLICATION = 0x81,
    ///Reset Communication
    RESET_COMMUNICATION = 0x82,
};

///Address 0 is to ALL nodes
const int ANYCANOPENNODE = 0;

///CANopen node modes
//TODO use it in the state machine?
enum CANopenNodeMode
{
    IS_INITIALIZATED,
    IS_PREOPERATIONAL,
    IS_OPERATIONAL,
    IS_STOPPED,
    UNKOWN,
};

///CANopen node events
//TODO use it in the state machine?
enum CANopenNodeEvent
{
    SET_PREOP,
    SET_OP,
    SET_STOP,
    SET_INIT,
};

///Enum for PDO types
enum PDOType
{
    ///Transmit type
    TX,
    ///Receive type
    RX
};

///CANopen data types according Maxon Documentation
enum CANopenDataType: uint8_t
{
    ///int8 type (0x02)
    INT8 = 0x02,
    ///int16 type (ox03)
    INT16 = 0x03,
    ///int32 type (0x04)
    INT32 = 0x04,
    
    ///uint32 type (0x05)
    UINT8 = 0x05,
    ///uint16 type (0x06)
    UINT16 = 0x06,
    ///uint32 type (0x07)
    UINT32 = 0x07,
    
    ///string type (0x09)
    STRING = 0x09,
    ///uint64 type (0x1B)
    UINT64 = 0x1B,
};

/// Turn DataType enumeration into string
/// \param type data type enum
/// \return readable string representing data type
std::string CANopenDataTypeToString(CANopenDataType type)
{
    std::string ret;
    
    switch(type)
    {
        case CANopenDataType::INT8:
            ret = "INT8";
            break;
        case CANopenDataType::INT16:
            ret = "INT16";
            break;
        case CANopenDataType::INT32:
            ret = "INT32";
            break;
        case CANopenDataType::UINT8:
            ret = "UINT8";
            break;
        case CANopenDataType::UINT16:
            ret = "UINT16";
            break;
        case CANopenDataType::UINT32:
            ret = "UINT32";
            break;
        case CANopenDataType::STRING:
            ret = "STRING";
            break;
        case CANopenDataType::UINT64:
            ret = "UINT64";
            break;
    }
    
    ret;
}

#endif //PASSIVITY_PROJECT_CANOPENTYPES_H
