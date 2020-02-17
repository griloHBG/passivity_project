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

enum FUNC_CODE: uint8_t
{
    NMT         = 0x00,
    SYNC        = 0x01,
    EMCY        = 0x01,
    TIME        = 0x02,
    
    SEND_PDO1   = 0x03,
    RECV_PDO1   = 0x04,
    SEND_PDO2   = 0x05,
    RECV_PDO2   = 0x06,
    SEND_PDO3   = 0x07,
    RECV_PDO3   = 0x08,
    SEND_PDO4   = 0x09,
    RECV_PDO4   = 0x10,
    
    SEND_SDO    = 0x11,
    RECV_SDO    = 0x12,
    
    HEARTBEAT = 0x14,
};

enum NMTCOMMAND: uint8_t
{
    OPERATIONAL = 0x01,
    STOPPED = 0x02,
    PREOPERATIONAL = 0x80,
    RESET_APPLICATION = 0x81,
    RESET_COMMUNICATION = 0x82,
};

const int ANYCANOPENNODE = 0;

enum CANopenNodeMode
{
    IS_INITIALIZATED,
    IS_PREOPERATIONAL,
    IS_OPERATIONAL,
    IS_STOPPED,
    UNKOWN,
};

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

#endif //PASSIVITY_PROJECT_CANOPENTYPES_H
