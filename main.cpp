#include <iostream>


#include <chrono>
#include <thread>
#include <omp.h>
#include <iomanip>
#include <map>

#include "utils.h"

#include "NodePDO.h"
#include "CANopenNode.h"
#include "CANopenCommunication.h"
#include <cmath>
#include <cstring>
#include <getopt.h>

#define SLEEP_ms(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

std::string printCANframe(const CANframe& frame)
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

int main(int argc, char** argv)
{
    int opt;
    bool print = false;
    bool maxTime_ms = 2000;
    
    /*char a[] = ["coisa", "-p"];
    char c[] = "-t";
    char d[] = "3000";
    
    argv = &a;
    
    while(opt=getopt(argc, argv, "pt:")!=-1)
    {
        switch (opt)
        {
            case 'p':
                print = true;
                break;
            case 't':
                maxTime_ms = atoi(optarg);
                break;
            default: // '?'
                fprintf(stderr, "Usage: %s [-p] [-t msecs]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }*/
    
    std::cout.setf(std::ios::unitbuf);
    
    CANopenNode node1(0x01,
                {
                       NodePDO(PDOType::TX, 0x181, {PDOpart(0, "Position Actual Value", CANopenDataType::INT32),
                                                    PDOpart(4, "Current Actual Value", CANopenDataType::INT16),
                                                    PDOpart(6, "StatusWord", CANopenDataType::UINT16)}),
 
                       NodePDO(PDOType::TX, 0x281, {PDOpart(0, "Velocity Actual Value", CANopenDataType::INT32)}),
 
                       NodePDO(PDOType::TX, 0x381, {PDOpart(0, "StatusWord", CANopenDataType::UINT16),
                                                    PDOpart(4, "Position Actual Value", CANopenDataType::INT32)}),
 
                       NodePDO(PDOType::TX, 0x481, {PDOpart(0, "StatusWord", CANopenDataType::UINT16),
                                                    PDOpart(2, "Velocity Actual Value", CANopenDataType::INT32)})},
                {
                       NodePDO(PDOType::RX, 0x201, {PDOpart(0, "PositionMode Setting Value", CANopenDataType::INT32),
                                                    PDOpart(4, "CurrentMode Setting Value", CANopenDataType::INT16),
                                                    PDOpart(6, "ControlWord", CANopenDataType::UINT16)}),
        
                       NodePDO(PDOType::RX, 0x301, {PDOpart(0, "VelocityMode Setting Value", CANopenDataType::INT32)}),
        
                       NodePDO(PDOType::RX, 0x401, {PDOpart(0, "ControlWord", CANopenDataType::UINT16),
                                                    PDOpart(2, "Target Position", CANopenDataType::INT32)}),
        
                       NodePDO(PDOType::RX, 0x501, {PDOpart(0, "ControlWord", CANopenDataType::UINT16),
                                                    PDOpart(2, "Target Velocity", CANopenDataType::INT32)})});
    
    CANopenCommunication canopenComm("can1", true);
    
    SLEEP_ms(500);
    
    canopenComm.AppendNode(node1);
    
    std::cout << "Sending RESET_APPLICATION" << std::endl;
    canopenComm.sendNMT(ANYCANOPENNODE, NMTCOMMAND::RESET_APPLICATION);
    SLEEP_ms(50);
    
    std::cout << "Sending RESET_COMMUNICATION" << std::endl;
    canopenComm.sendNMT(ANYCANOPENNODE, NMTCOMMAND::RESET_COMMUNICATION);
    SLEEP_ms(50);
    
    std::cout << "Sending PREOPERATIONAL" << std::endl;
    canopenComm.sendNMT(ANYCANOPENNODE, NMTCOMMAND::PREOPERATIONAL);
    SLEEP_ms(100);
    
    //Setting Position control
    
    //Set mode of opration (0x6060-00) to Position mode (0xFF)
    std::cout << printCANframe(canopenComm.sendSDO(node1, true, 0x6060, 0x00, CANopenDataType::INT8, 0xFF));
    SLEEP_ms(100);
    //ControlWord (0x6040-00) to shutdown (0x0006)
    std::cout << printCANframe(canopenComm.sendSDO(node1, true, 0x6040, 0x00, CANopenDataType::INT16, 0x0006));
    SLEEP_ms(100);
    //ControlWord (0x6040-00) to SwitchOn (0x000F)
    std::cout << printCANframe(canopenComm.sendSDO(node1, true, 0x6040, 0x00, CANopenDataType::INT16, 0x000F));
    SLEEP_ms(100);
    //Set Position Mode Setting Value (0x2062)
    std::cout << printCANframe(canopenComm.sendSDO(node1, true, 0x2062, 0x00, CANopenDataType::INT32, 0));
    
    std::cout << "print? R: " << (print ? "YES!" : "NO!") << std::endl;
    
    int64_t startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    int64_t myTime = 0;
    
    for(int p = 0 ; p < 4000 && myTime < maxTime_ms; p++)
    {
        if(print)
        {
            std::cout << printCANframe(canopenComm.sendSDO(node1, true, 0x2062, 0x00, CANopenDataType::INT32, round(200*sin(p/50.0))));
        }
        else
        {
            std::cout << std::setw(5) << myTime << std::setw(10) << 200*sin(myTime * M_PI/1000.0) << std::endl;
            myTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startTime;
            canopenComm.sendSDO(node1, true, 0x2062, 0x00, CANopenDataType::INT32, round(200*sin(myTime * M_PI/1000.0)));
            p--;
        }
        
    }
    
    std::cout << myTime << std::endl;
    
    SLEEP_ms(100);
    
    std::cout << "Sending OPERATIONAL" << std::endl;
    canopenComm.sendNMT(ANYCANOPENNODE, NMTCOMMAND::OPERATIONAL);
    SLEEP_ms(50);

    int msg_count = 0;
    
    while(msg_count < 100)
    {
        if(node1.hasNews())
        {
            msg_count++;
            std::cout << node1.allPDOtoString().str() << std::endl;
        }
        else
        {
            std::this_thread::yield();
        }
    }
    
    std::cout << "Sending STOPPED" << std::endl;
    canopenComm.sendNMT(ANYCANOPENNODE, NMTCOMMAND::STOPPED);
    SLEEP_ms(2000);
    
    return 0;
}
