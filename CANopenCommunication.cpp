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
    // actually defining the socket
    _commSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    // writing down the interface name
    strcpy(_ifreq.ifr_name, _device.c_str());
    
    // getting the interface index from ifreq and setting socket to it
    // from man:
    //  SIOCGIFINDEX (perhaps System IO Control Get InterFace INDEX?)
    //               Retrieve the interface index of the interface into
    //               ifr_ifindex.
    // and: ioctl is a do-a-lot function
    ioctl(_commSocket, SIOCGIFINDEX, &(_ifreq));
    
    // setting address to the right Address Family (AF): AF_CAN
    _address.can_family = AF_CAN;
    // setting interface index of address
    _address.can_ifindex = _ifreq.ifr_ifindex;
    
    // binding address to socket
    bind(_commSocket, (Address*)&(_address), sizeof(_address));
    
    // a flag that helps to stop the receive thread
    _keepReceiveAlive = true;
    
    // stating the receive thread!
    _threadRecv = std::thread(&CANopenCommunication::_recvThread, this);
}

CANopenCommunication::CANopenCommunication(const std::string &device, bool startComm = false) : _device(device)
{
    // setting each payload length of each message
    _rxNMT.can_dlc = 2; // 2 bytes
    _txNMT.can_dlc = 2; // 2 bytes
    
    _rxPDO.can_dlc = 8; // 8 bytes
    _txPDO.can_dlc = 8; // 8 bytes
    
    _rxSDO.can_dlc = 8; // 8 bytes
    _txSDO.can_dlc = 8; // 8 bytes
    
    // start communication if assked
    if(startComm)
    {
        startCommunication();
    }
}

bool CANopenCommunication::sendNMT(int node, NMTCOMMAND command)
{
    //TODO maybe don't spend processing with each NMT message? pre-made them?
    
    //TODO mutex is really needed to NMT messages?
    
    // lock NMT message mutex
    _txNMT_mtx.lock();
    // clear NMT message data
    memset(_txNMT.data, 0, 2*sizeof(uint8_t));
    
    // max of 127 nodes!
    if (node > MAX_NODES_NUM)
    {
        std::cout << "There can't be more than 127 nodes! (node = " << node << ")" << std::endl;
        return false;
    }
    
    // can_id for NMT message
    _txNMT.can_id = (FUNC_CODE::NMT << 7);
    
    // set command and node address
    _txNMT.data[0] = command;
    _txNMT.data[1] = node;
    
    // finally write it to the CAN bus
    write(_commSocket, &(_txNMT), sizeof(CANframe));
    
    // unlock NMT message
    _txNMT_mtx.unlock();
    
    return false;
}

void CANopenCommunication::_recvThread()
{
    // timeout to select call
    struct timeval tv{0, 1000}; //1 millissecond
    
    // select return value
    int selectRet;
    // set of read file descriptors that select call will watch
    fd_set readfds;
    // amount if bytes received from CAN bus read
    int nbytes;
    
    // while this thread must be alive
    while(_keepReceiveAlive)
    {
        // lock socket mutex
        _commSocket_mtx.lock();
        // lock internal receive CANframe buffer
        _rxinternalCANframe_mtx.lock();
        
        // reset set of read file descriptors
        FD_ZERO(&readfds);
        // add socket file descriptor to readfds set
        FD_SET(_commSocket, &readfds);
        // wait for something to be readable at readfds or until the tv timeout
        selectRet = select(_commSocket + 1, &readfds, NULL, NULL, &tv);
        
        // if return from select if -1, something went very wrong.
        if(selectRet == -1)
        {
            _rxinternalCANframe_mtx.unlock();
            _commSocket_mtx.unlock();
            throw std::runtime_error("select call returned -1 (CANopenCommunication.cpp:" + std::to_string(__LINE__) + ")!\n");
        }
        // else, lets see it.
        else if (selectRet > 0)
        {
            // read CAN bus
            nbytes = read(_commSocket, &_rxinternalCANframe, sizeof(CANframe));
            // unlock socket mutex
            _commSocket_mtx.unlock();
            // if something was read
            if(nbytes > 0)
            {
                // read the address
                uint8_t nodeAddress = (_rxinternalCANframe.can_id & 0x7F);
                
                // update node with the respective address
                //TODO evaluate _rxinternalCANframe's address
                _nodes.at(nodeAddress)->update(std::move(_rxinternalCANframe));
        
            }
        }
        //here, nothing (in readfds, in this case, the CAN socket) was ready to read
        else //selectRet == 0
        {
            // unlock socket mutex
            _commSocket_mtx.unlock();
        }
        
        // unlock internal read frame mutex
        _rxinternalCANframe_mtx.unlock();
    }
}

bool CANopenCommunication::AppendNode(CANopenNode& node)
{
    // it's straightforward
    // TODO does it work?
    if(_nodes.at(node.getAddress()) == nullptr)
        _nodes.at(node.getAddress()) = &node;
}

CANopenCommunication::~CANopenCommunication()
{
    // on destruction
    // let receive die
    _keepReceiveAlive = false;
    // wait for it to join
    _threadRecv.join();
    // close communication socket
    close(_commSocket);
}

CANframe
CANopenCommunication::sendSDO(const CANopenNode& node, const bool& doWrite, const uint16_t& index, const uint8_t& subindex, const CANopenDataType& dataType = CANopenDataType::INT32, const int& value = 0)
{
    // flag for error at data
    bool dataError = false;
    
    // amount of data inn bytes that will not be used in this SDO message (related to n value of the SDO message)
    uint8_t invalidData = 4;
    
    // analise the type of data
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
    SDOmessage.can_id = 0x600 + node.getAddress();
    
    // alias for the first byte of data (command byte for SDO message)
    uint8_t& commandByte = SDOmessage.data[0];
    
    // set object index
    SDOmessage.data[1] = index & 0xFF;
    SDOmessage.data[2] = index >> 8;
    
    // set object subindex
    SDOmessage.data[3] = subindex;
    
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
    
    // lock socket mutex
    _commSocket_mtx.lock();
    
    // write to CAN bus
    write(_commSocket, &SDOmessage, sizeof(CANframe));
    
    // unlock socket mutex
    _commSocket_mtx.unlock();
    
    return SDOmessage;
}

