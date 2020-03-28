//
// Created by grilo on 28/01/2020.
//

#ifndef PASSIVITY_PROJECT_CANOPENCOMMUNICATION_H
#define PASSIVITY_PROJECT_CANOPENCOMMUNICATION_H

#include <string>
#include <sys/socket.h>         //socket(), AF_CAN,
#include <linux/can.h>          //struct can_frame, struct sockaddr_can,
#include <net/if.h>             //struct ifreq,
#include <cstring>             //strcpy(),
#include <sys/ioctl.h>          //ioctl(), SIOCGIFINDEX,
#include <mutex>                //mutex
#include <thread>               //thread
#include <vector>               //vector
#include "CANopenNode.h"

class CANopenCommunication {
public:
    ///maximum amount of nodes in a CANopen network
    static const uint8_t MAX_NODES_NUM = 0x7F;
    
    /// Ctor for CANopen communication
    /// \param device name of device (e.g. can0, can1, dcan0, vcan1 etc)
    /// \param startComm should the communication be start?
    CANopenCommunication(const std::string &device, bool startComm = false);
    
    /// Dtor
    ~CANopenCommunication();
    
    /// Function to append node to the network
    /// \param node reference to node
    /// \return indicator if node was added
    //TODO return is working?
    bool AppendNode(CANopenNode *node);
    
    /// send NMT message with command to node
    /// \param node node to receive the command
    /// \param command command to send to node
    /// \return not used
    //TODO maybe use the return?
    bool sendNMT(int node, NMTCOMMAND command);
    
    /// send PDO message to node
    /// \param node node to send PDO message
    /// \param PDOnumber which PDO to send to node
    /// \return dont know
    //TODO implement it? maybe only in a possible child of this class?
    bool sendPDO(int node, int PDOnumber);
    
    /// Send SDO message to node
    /// \param node which node
    /// \param doWrite write (download from client to server) or read (upload from server to client)
    /// \param index object index from dictionary
    /// \param subindex object subindex from dictionary
    /// \param dataType data type to be sent or read
    /// \param value value to be sent
    /// \return the CANframe that can be sent through CAN bus
    CANframe sendSDO(const CANopenNode &node, const bool &doWrite, const IndexSubindex& indexSubIndex, const CANopenDataType &dataType, const int &value);
    
    /// receive NMT message (needed?)
    /// \param node which node (???)
    /// \return what??
    //TODO necessary?
    bool recvNMT(int node);
    
    /// receive SDO message (?!)
    /// \param node
    /// \return
    //TODO what to do with it?
    bool recvSDO(int node);
    
    /// actually start the communication
    /// \return
    bool startCommunication();

private:
    
    ///Every node that can be added to the network
    std::array<CANopenNode*, 127> _nodes;
    
    ///linux CAN device name (like can0, can1, dcan0 etc)
    std::string _device;
    
    ///the socket per se
    Socket _commSocket;
    ///address struct used to setup socket
    CANaddress _address;
    ///interface request (?) to setup socket
    InterfRequest _ifreq;
    ///current PDO message received
    CANframe _txPDO;
    ///current PDO message to be transmited
    CANframe _rxPDO;
    ///current SDO message to be transmited
    CANframe _txSDO;
    ///current SDO message received
    CANframe _rxSDO;
    ///current NMT message to be transmited
    CANframe _txNMT;
    ///current NMT message received
    CANframe _rxNMT;
    ///TODO variable used to transmit any CAN message. Is this needed?!
    CANframe _txinternalCANframe;
    ///variable that receives any CAN message received. Always overrided by the next reception
    CANframe _rxinternalCANframe;
    
    ///mutex for _txinternalCANframe
    std::mutex _txinternalCANframe_mtx;
    ///mutex for _rxinternalCANframe
    std::mutex _rxinternalCANframe_mtx;
    ///mutex for _txNMT
    std::mutex _txNMT_mtx;
    ///mutex for _rxNMT
    std::mutex _rxNMT_mtx;
    ///mutex for _txSDO
    std::mutex _txSDO_mtx;
    ///mutex for _rxSDO
    std::mutex _rxSDO_mtx;
    ///mutex for _txPDO
    std::mutex _txPDO_mtx;
    ///mutex for _rxPDO
    std::mutex _rxPDO_mtx;
    ///mutex for _commSocket
    std::mutex _commSocket_mtx;
    
    ///thread object to hold receive thread
    std::thread _threadRecv;
    
    bool _keepReceiveAlive;
    
    ///the receive function for the receive thread
    void _recvThread();
};


#endif //PASSIVITY_PROJECT_CANOPENCOMMUNICATION_H









