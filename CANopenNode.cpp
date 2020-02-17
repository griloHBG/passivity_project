//
// Created by grilo on 29/01/2020.
//

#include "CANopenNode.h"
#include <cassert>
#include <sstream>
#include <iomanip>

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

CANopenNode::CANopenNode(uint8_t address,
                        std::array<NodePDO, 4>&& txPdOarray,
                        std::array<NodePDO, 4>&& rxPdOarray):
        _address(address), _txPDOarrayInterface(txPdOarray),
        _rxPDOarrayInterface(rxPdOarray)
{}

uint8_t CANopenNode::getAddress() const
{
    return _address;
}CANopenNode::CANopenNode(const CANopenNode &otherNode)
        : _address(otherNode._address), _mode(otherNode._mode), _rxPDOarrayInterface(otherNode._rxPDOarrayInterface), _txPDOarrayInterface(otherNode._txPDOarrayInterface)
{
    throw std::invalid_argument("COOOPY! (CANopenNode.cpp:" + std::to_string(__LINE__));
}

void CANopenNode::update(CANframe frame)
{
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
}

void CANopenNode::update(CANframe&& frame)
{
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
}

bool CANopenNode::hasNews()
{
    return _hasNews;
}

std::stringstream CANopenNode::PDOproperties()
{
    std::stringstream ret;
    
    int pdoIndex = 0, partIndex = 0;
    
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
