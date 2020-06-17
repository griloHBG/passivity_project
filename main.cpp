#include <iostream>


#include <chrono>
#include <thread>
#include <omp.h>
#include <map>
#include <cmath>
#include <cstring>
#include <getopt.h>
#include "CANopenCommunication.h"
#include "CANopenUtils.h"
#include "EPOSnode.h"

#define SLEEP_ms(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

int main(int argc, char** argv)
{
    int opt;
    bool print = false;
    int maxTime_ms = 2000;
    
    /*char a[] = {"coisa", "-p"};
    char c[] = "-t";
    char d[] = "3000";
    
    argv = &a;*/
    
    while((opt=getopt(argc, argv, "pt:"))!=-1)
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
    }
    
    std::cout.setf(std::ios::unitbuf);
    
    EPOSnode node1(0x01);
    
    CANopenCommunication canopenComm("can1");
    
    canopenComm.AppendNode(&node1);
    
    canopenComm.startCommunication();
    
    SLEEP_ms(500);
    
    std::cout << "Sending RESET_APPLICATION" << std::endl;
    canopenComm.sendNMT(ANYCANOPENNODE, NMTCOMMAND::RESET_APPLICATION);
    SLEEP_ms(50);
    
    std::cout << "Sending RESET_COMMUNICATION" << std::endl;
    canopenComm.sendNMT(ANYCANOPENNODE, NMTCOMMAND::RESET_COMMUNICATION);
    SLEEP_ms(50);
    
    std::cout << "Sending PREOPERATIONAL" << std::endl;
    canopenComm.sendNMT(ANYCANOPENNODE, NMTCOMMAND::PREOPERATIONAL);
    SLEEP_ms(100);
    
    std::cout << "Setting Position control" << std::endl;
    
    std::cout << "Set mode of operation (0x6060-00) to Position mode (0xFF)" <<std::endl;
    std::cout << printCANframe(canopenComm.sendSDO(node1, true, 0x606000, CANopenDataType::INT8, 0xFF));
    SLEEP_ms(100);
    std::cout << "ControlWord (0x6040-00) to shutdown (0x0006)" << std::endl;
    std::cout << printCANframe(canopenComm.sendSDO(node1, true, 0x604000, CANopenDataType::INT16, 0x0006));
    SLEEP_ms(100);
    std::cout << "ControlWord (0x6040-00) to SwitchOn (0x000F)" << std::endl;
    std::cout << printCANframe(canopenComm.sendSDO(node1, true, 0x604000, CANopenDataType::INT16, 0x000F));
    SLEEP_ms(100);
    std::cout << "Set Position Mode Setting Value (0x2062-00) to 0" << std::endl;
    std::cout << printCANframe(canopenComm.sendSDO(node1, true, 0x206200, CANopenDataType::INT32, 0));
    
    std::cout << "print? R: " << (print ? "YES!" : "NO!") << std::endl;
    
    int64_t startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    int64_t myTime = 0;
    int p = 0;
    int value;
    //for(int p = 0 ; p < 4000 && myTime < maxTime_ms; p++)
    for(;;)
    {
        if(print)
        {
            std::cout << printCANframe(canopenComm.sendSDO(node1, true, 0x206200, CANopenDataType::INT32, round(800 * sin(p / 50.0))));
            p++;
            if(p > 4000)
            {
                break;
            }
        }
        else
        {
            value = round(200 * sin(myTime * M_PI / 1000.0));
            myTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startTime;
            canopenComm.sendSDO(node1, true, 0x206200, CANopenDataType::INT32, value);
            if(myTime > maxTime_ms)
            {
                std::cout << "\n\nTIIIIME!\nmyTime " << myTime  << "\n";
                break;
            }
        }
    }
    
    std::cout << myTime << std::endl;
    
    SLEEP_ms(100);
    
    std::cout << "Sending OPERATIONAL" << std::endl;
    canopenComm.sendNMT(ANYCANOPENNODE, NMTCOMMAND::OPERATIONAL);
    SLEEP_ms(50);

    std::cout << node1.allPDOtoString().str() << std::endl;
    
    std::cout << "Sending STOPPED" << std::endl;
    canopenComm.sendNMT(ANYCANOPENNODE, NMTCOMMAND::STOPPED);
    
    std::cout << "Sending RESET NODE" << std::endl;
    canopenComm.sendNMT(ANYCANOPENNODE, NMTCOMMAND::RESET_APPLICATION);
    
    SLEEP_ms(2000);
    
    return 0;
}
