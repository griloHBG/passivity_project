//
// Created by grilo on 29/01/2020.
//

#include "CANopenNode.h"
#include "CANopenUtils.h"
#include <sstream>
#include <iomanip>
#include <memory>
#include <utility>

/*
bool CANopenNode::Transition(const CANopenNodeEvent event)
{
    bool result = false;
    switch (event)
    {
        case CANopenNodeEvent::SET_INIT:
            if (this->_mode != CANopenNodeMode::IS_OPERATIONAL && this->_mode != CANopenNodeMode::IS_STOPPED){
                result = false;
            }
            else
            {
            
            }
            break;
        case CANopenNodeEvent::SET_OP:
            break;
        case CANopenNodeEvent::SET_PREOP:
            break;
        case CANopenNodeEvent::SET_STOP:
            break;
    }
    return false;
}
*/

std::array<unsigned int, 4> CANopenNode::getTxPDO_COB_ID() const
{
    std::array<unsigned int, 4> cob_ids{};
    
    for(int i = 0; i < 4; i++)
    {
        cob_ids.at(i) = _txPDOarrayInterface.at(i).getAddress();
    }
    return cob_ids;
}

std::array<unsigned int, 4> CANopenNode::getRxPDO_COB_ID() const
{
    std::array<unsigned int, 4> cob_ids{};
    
    for(int i = 0; i < 4; i++)
    {
        cob_ids.at(i) = _rxPDOarrayInterface.at(i).getAddress();
    }
    return cob_ids;
}

CANopenNode::CANopenNode(uint8_t address)
{
    _address = address;
    _isConfigured = false;
}

void CANopenNode::configurePDOs(std::array<PDOmessage, 4> txPDOarrayInterface,
                                std::array<PDOmessage, 4> rxPDOarrayInterface)
{
    _txPDOarrayInterface = std::move(txPDOarrayInterface);
    _rxPDOarrayInterface = std::move(rxPDOarrayInterface);
    
    std::vector<PDOpart> vecPDOpart;
    int iPDOmsg, iPDOpart;
    
    for(iPDOmsg = 0; iPDOmsg < _txPDOarrayInterface.size(); iPDOmsg++)
    {
        vecPDOpart = _txPDOarrayInterface[iPDOmsg].getPayloadMap();
        for(iPDOpart = 0; iPDOpart < vecPDOpart.size(); iPDOpart++)
        {
            _txPDOvalues[iPDOmsg].push_back(vecPDOpart[iPDOpart].getDictionaryEntryValueRef());
        }
    }
    
    for(iPDOmsg = 0; iPDOmsg < _rxPDOarrayInterface.size(); iPDOmsg++)
    {
        vecPDOpart = _rxPDOarrayInterface[iPDOmsg].getPayloadMap();
        for(iPDOpart = 0; iPDOpart < vecPDOpart.size(); iPDOpart++)
        {
            _rxPDOvalues[iPDOmsg].push_back(vecPDOpart[iPDOpart].getDictionaryEntryValueRef());
        }
    }
    
    _isConfigured = true;
}

/*CANopenNode::CANopenNode(const CANopenNode &otherNode)
        : _address(otherNode._address), _mode(otherNode._mode), _rxPDOarrayInterface(otherNode._rxPDOarrayInterface), _txPDOarrayInterface(otherNode._txPDOarrayInterface)
{
    throw std::invalid_argument("COOOPY! (CANopenNode.cpp:" + std::to_string(__LINE__));
}*/

uint8_t CANopenNode::getAddress() const
{
    return _address;
}

//copying frame
/*void CANopenNode::update(CANframe frame)
{
    // store the received frame from CAN bus to in the right place
    switch(frame.can_id >> 7)
    {
        case SEND_PDO1:
            _txPDOvalues.at(0) = _txPDOarrayInterface.at(0).valuesFromPayload(frame);
            break;
        case SEND_PDO2:
            _txPDOvalues.at(1) =_txPDOarrayInterface.at(1).valuesFromPayload(frame);
            break;
        case SEND_PDO3:
            _txPDOvalues.at(2) = _txPDOarrayInterface.at(2).valuesFromPayload(frame);
            break;
        case SEND_PDO4:
            _txPDOvalues.at(3) =_txPDOarrayInterface.at(3).valuesFromPayload(frame);
            break;
        case SEND_SDO:
            //TODO
            break;
    }
}*/

void CANopenNode::update(CANframe&& frame)
{
    if(_isConfigured)
    {
        //TODO evaluate frame's address
        std::vector<int> auxPayload;
        int PDOindex = 0;
        switch (frame.can_id >> 7)
        {
            case SEND_PDO1:
                PDOindex = 0; //for PDO1
                break;
            case SEND_PDO2:
                PDOindex = 1; //for PDO2
                break;
            case SEND_PDO3:
                PDOindex = 2; //for PDO3
                break;
            case SEND_PDO4:
                PDOindex = 3; //for PDO4
                break;
            case SEND_SDO:
                //TODO
                updateSDOobject(frame);
                return;
                break;
            default:
                return;
                break;
            /*case RECV_SDO:
                //TODO
                break;*/
        }
    
        auxPayload = _txPDOarrayInterface.at(PDOindex).valuesFromPayload(frame);
        
        for(int i = 0; i < auxPayload.size(); i++)
        {
            _txPDOvalues.at(PDOindex).at(i) = auxPayload.at(i);
        }
    }
    else
    {
        throw std::runtime_error("This node is not configured! (CANopenNode.cpp:" + std::to_string(__LINE__) + ")\n");
    }
}

bool CANopenNode::hasNews()
{
    // a lot useless. for now
    //TODO THIS IS USELESS
    return _hasNews;
}

std::stringstream CANopenNode::allPDOtoString()
{
    // the returning string stream
    std::stringstream ret;
    
    // counters
    int pdoIndex = 0, partIndex = 0;
    
    // for each
    for(pdoIndex = 0; pdoIndex < 4; pdoIndex++)
    {
        for(partIndex = 0; partIndex < _txPDOvalues.at(pdoIndex).size(); partIndex++)
        {
            ret << std::setw(20) << _txPDOarrayInterface.at(pdoIndex).at(partIndex).getDescription();
            ret << std::setw(10) << _txPDOvalues.at(pdoIndex).at(partIndex);
        }
        //TODO continue from here
    }
    return ret;
}

CANframe
CANopenNode::createSDO(const bool &doWrite, const DictionaryEntry& entry, const int &value)
{
    return ::createSDO(this->_address, doWrite, std::get<INDEXSUBINDEX>(entry), std::get<TYPE>(entry), std::get<VALUE>(entry));
}
