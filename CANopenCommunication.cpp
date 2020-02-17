//
// Created by grilo on 28/01/2020.
//

#include <iostream>
#include "CANopenCommunication.h"
#include <unistd.h>
#include <vector>
#include <thread>

bool CANopenCommunication::startCommunication()
{
    _commSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    strcpy(_ifreq.ifr_name, _device.c_str());
    ioctl(_commSocket, SIOCGIFINDEX, &(_ifreq));
    _address.can_family = AF_CAN;
    _address.can_ifindex = _ifreq.ifr_ifindex;
    bind(_commSocket, (Address*)&(_address), sizeof(_address));
    
    _keepReceiveAlive = true;
    _threadRecv = std::thread(&CANopenCommunication::_recvThread, this);
}

CANopenCommunication::CANopenCommunication(const std::string &device, bool startComm = false) : _device(device)
{
    _rxNMT.can_dlc = 2; // 2 bytes
    _txNMT.can_dlc = 2; // 2 bytes
    
    _rxPDO.can_dlc = 8; // 8 bytes
    _txPDO.can_dlc = 8; // 8 bytes
    
    _rxSDO.can_dlc = 8; // 8 bytes
    _txSDO.can_dlc = 8; // 8 bytes
    
    if(startComm)
    {
        startCommunication();
    }
}

bool CANopenCommunication::sendNMT(int node, NMTCOMMAND command)
{
    _txNMT_mtx.lock();
    memset(_txNMT.data, 0, 2*sizeof(uint8_t));
    
    //max of 127 nodes!
    if (node > MAX_NODES_NUM)
    {
        std::cout << "There can't be more than 127 nodes! (node = " << node << ")" << std::endl;
        return false;
    }
    
    _txNMT.can_id = (FUNC_CODE::NMT << 7);
    
    _txNMT.data[0] = command;
    _txNMT.data[1] = node;
    
    write(_commSocket, &(_txNMT), sizeof(CANframe));
    
    _txNMT_mtx.unlock();
    
    return false;
}

void CANopenCommunication::_recvThread()
{
    
    struct timeval tv{0, 1000}; //1 millissecond
    int retval;
    fd_set readfds;
    int nbytes;
    
    while(_keepReceiveAlive)
    {
        _commSocket_mtx.lock();
        _rxinternalCANframe_mtx.lock();
    
        FD_ZERO(&readfds);
        FD_SET(_commSocket, &readfds);
        //TODO: is this working?
        retval = select(_commSocket+1, &readfds, NULL, NULL, &tv);
        
        if(retval == -1)
        {
            _rxinternalCANframe_mtx.unlock();
            _commSocket_mtx.unlock();
            throw std::runtime_error("select call returned -1 (CANopenCommunication.cpp:" + std::to_string(__LINE__) + ")!\n");
        }
        else if (retval > 0)
        {
            nbytes = read(_commSocket, &_rxinternalCANframe, sizeof(CANframe));
            _commSocket_mtx.unlock();
            if(nbytes > 0)
            {
        
                uint8_t nodeAddress = (_rxinternalCANframe.can_id & 0x7F);
        
                _nodes.at(nodeAddress)->update(_rxinternalCANframe);
        
            }
        }
        else //retval == 0
        {
            _commSocket_mtx.unlock();
        }
        

        _rxinternalCANframe_mtx.unlock();
    }
}

bool CANopenCommunication::AppendNode(CANopenNode& node)
{
    _nodes.at(node.getAddress()) = &node;
}

void CANopenCommunication::_messageOrganizer()
{
    while(_keepReceiveAlive)
    {
        std::this_thread::yield();
    }
}

CANopenCommunication::~CANopenCommunication()
{
    _keepReceiveAlive = false;
    _threadRecv.join();
}

CANframe
CANopenCommunication::sendSDO(const CANopenNode& node, const bool& doWrite, const uint16_t& index, const uint8_t& subindex, const CANopenDataType& dataType = CANopenDataType::INT32, const int& value = 0)
{
    bool dataError = false;
    
    uint8_t invalidData = 4;
    
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
        default:
            //if read
            if(doWrite)
            {
                dataError = true;
            }
    }
    
    if(dataError)
    {
        throw std::invalid_argument("maximum type expected is 4 bytes! (CANopenCommunication.cpp:" + std::to_string(__LINE__) + ")\n");
    }
    
    CANframe SDOmessage;
    
    SDOmessage.can_dlc = 8;
    
    SDOmessage.can_id = 0x600 + node.getAddress();
    
    uint8_t& commandByte = SDOmessage.data[0];
    
    SDOmessage.data[1] = index & 0xFF;
    SDOmessage.data[2] = index >> 8;
    SDOmessage.data[3] = subindex;
    
    std::array<uint8_t, 4> data{};
    
    if (doWrite) //SDO download
    {
        //client command specifier (ccs)
        uint8_t ccs = 1;
        //message is not segmented!
        uint8_t expedited = 1;
        //size IS indicated
        uint8_t size = 1;
        
        commandByte = (ccs << 5) | (invalidData << 2) | (expedited << 1) | size;
        
        for(int i = 0; i < (4 - invalidData); i++)
        {
            data.at(i) = (value >> 8*i) & (0xFF);
        }
    }
    else // read: SDO upload
    {
        //client command specifier (ccs)
        uint8_t ccs = 2;
    
        commandByte = (ccs << 5);
    }
    
    SDOmessage.data[0] = commandByte;
    
    for(int i = 0; i < 4; i++)
    {
        SDOmessage.data[i+4] = data.at(i);
    }
    
    _commSocket_mtx.lock();
    
    //TODO: sometimes doesnt writes...
    write(_commSocket, &SDOmessage, sizeof(CANframe));
    
    _commSocket_mtx.unlock();
    
    return SDOmessage;
}

