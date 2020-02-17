//
// Created by grilo on 28/01/2020.
//

#ifndef PASSIVITY_PROJECT_CANOPENCOMMUNICATION_H
#define PASSIVITY_PROJECT_CANOPENCOMMUNICATION_H

#include <string>
#include <sys/socket.h>         //socket(), AF_CAN,
#include <linux/can.h>          //struct can_frame, struct sockaddr_can,
#include <net/if.h>             //struct ifreq,
#include <string.h>             //strcpy(),
#include <sys/ioctl.h>          //ioctl(), SIOCGIFINDEX,
#include <mutex>
#include <thread>
#include <vector>
#include "CANopenNode.h"
#define IS_RXPDO(can_id)

class CANopenCommunication {
public:
    static const uint8_t MAX_NODES_NUM = 0x7F;
    CANopenCommunication(const std::string &device, bool startComm);
    ~CANopenCommunication();
    bool AppendNode(CANopenNode& node);
    bool sendNMT(int node, NMTCOMMAND command);
    bool sendPDO(int node, int PDOnumber);
    CANframe sendSDO(const CANopenNode& node, const bool& doWrite, const uint16_t& index, const uint8_t& subindex, const CANopenDataType& dataType, const int& value);
    bool recvNMT(int node);
    bool recvSDO(int node);
    
    bool startCommunication();

private:
    /* useless?
    std::array<CANopenNode*, 127> _usedRxPasdasdasdasdasdDO1;
    std::array<CANopenNode*, 127> _usedRxPDO2;
    std::array<CANopenNode*, 127> _usedRxPDO3;
    std::array<CANopenNode*, 127> _usedRxPDO4;
    
    std::array<CANopenNode*, 127> _usedTxPDO1;
    std::array<CANopenNode*, 127> _usedTxPDO2;
    std::array<CANopenNode*, 127> _usedTxPDO3;
    std::array<CANopenNode*, 127> _usedTxPDO4;
     */
    
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
    ///variable that receives any CAN message received. Always overrided by the next reception
    CANframe _rxinternalCANframe;
    ///TODO variable used to transmit any CAN message. Is this needed?!
    CANframe _txinternalCANframe;
    
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
    ///thread object to hold function message receiving
    std::thread _organizeMessasge;
    bool _keepReceiveAlive;
    
    ///the receive function for thre receive thread
    void _recvThread();
    ///function to organize received message
    void _messageOrganizer();
    
};


#endif //PASSIVITY_PROJECT_CANOPENCOMMUNICATION_H









