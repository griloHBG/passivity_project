//
// Created by grilo on 28/01/2020.
//

#include <iostream>
#include <unistd.h>
#include <vector>
#include <thread>
#include "CANopenCommunication.h"
#include "CANopenUtils.h"

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
    // although
    bind(_commSocket, (Address*)&(_address), sizeof(_address));
    
    // a flag that helps to stop the receive thread
    _keepReceiveAlive = true;
    
    // stating the receive thread!
    _threadRecv = std::thread(&CANopenCommunication::_recvThread, this, nullptr);
}

CANopenCommunication::CANopenCommunication(const std::string device, bool startComm) : _device(device), _address({0})
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
    // hoping this will be optimized out
    _txNMT.can_id = (FUNC_CODE::NMT << 7);
    
    // set command and node address
    _txNMT.data[0] = command;
    _txNMT.data[1] = node;
    
    _commSocket_mtx.lock();
    
    // finally write it to the CAN bus
    write(_commSocket, &(_txNMT), sizeof(CANframe));
    
    _commSocket_mtx.unlock();
    
    // unlock NMT message
    _txNMT_mtx.unlock();
    
    return false;
}


void CANopenCommunication::_recvThread(void *)
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
        
        // reset set of read file descriptors
        FD_ZERO(&readfds);
        // add socket file descriptor to readfds set
        FD_SET(_commSocket, &readfds);
        // wait for something to be readable at readfds or until the tv timeout
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        selectRet = select(_commSocket + 1, &readfds, NULL, NULL, &tv);
        
        // if return from select if -1, something went very wrong.
        if(selectRet == -1)
        {
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
                
                //TODO evaluate message for errors!!
                
                // update node with the respective address
                //TODO evaluate _rxinternalCANframe's address
                //TODO do not use .at because is slower (maybe compiling optimization helps with it?)
                _nodes.at(nodeAddress)->update(std::move(_rxinternalCANframe));
            }
        }
        //here, nothing (in readfds, in this case, the CAN socket _commSocket) was ready to read
        else //selectRet == 0
        {
            // unlock socket mutex
            _commSocket_mtx.unlock();
        }
    }
}

bool CANopenCommunication::AppendNode(CANopenNode *node)
{
    // it's straightforward
    // TODO does it work?
    _nodes.at(node->getAddress()) = node;
}

CANopenCommunication::~CANopenCommunication()
{
    // on destruction
    // let receive die
    _keepReceiveAlive = false;
    
    // if joinable
    if(_threadRecv.joinable())
    {
        // wait for it to join
        _threadRecv.join();
    }
    
    // close communication socket
    close(_commSocket);
}

CANframe
CANopenCommunication::sendSDO(const CANopenNode &node, const bool &doWrite, const IndexSubindex&indexSubIndex, const CANopenDataType &dataType, const int &value = 0)
{
    CANframe SDOmessage = createSDO(node.getAddress(), doWrite, indexSubIndex, dataType, value);
    
    // lock socket mutex
    _commSocket_mtx.lock();
    
    // write to CAN bus
    write(_commSocket, &SDOmessage, sizeof(CANframe));
    
    // unlock socket mutex
    _commSocket_mtx.unlock();
    
    return SDOmessage;
}

