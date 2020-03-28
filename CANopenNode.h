//
// Created by grilo on 29/01/2020.
//

#ifndef PASSIVITY_PROJECT_CANOPENNODE_H
#define PASSIVITY_PROJECT_CANOPENNODE_H
/*****************************************
 *
 *              Power-On
 *                   |
 *                   |
 *           +-------v-----------+
 *      +----> Initialization(0) <---------+
 *      |    +--------+----------+         |
 *      |             |                    |
 *      |             |                    |
 *      |    +--------v----------+         |
 *      |    | PreOperational(1) <------+  |
 *      |    +---+-------+--+----+      |  |
 *      |        |       |  |         +-+--+-------+
 *      |        |       |  +--------->            |
 *      |        |       |            | Stopped(3) |
 *      |        |       |  +---------+            |
 *      |        |       |  |         +-^----------+
 *      |     +--v-------v--v---+       |
 *      +-----+ Operational(2)  +-------+
 *            +-----------------+
 *
 *
 ****************************************/

#include <array>
#include <map>
#include <stdexcept>
#include <iostream>
#include <functional>       //referece_wrapper
#include "PDOmessage.h"
#include "CANopenTypes.h"

class CANopenNode
{
public:
    //TODO implement state machine for CANopen node?
    //bool Transition(CANopenNodeEvent event);
    
    /// getTxPDO_COB_ID to expose all of this COB_ID addresses
    /// \return array containing all 4 TxPDO COB_ID
    std::array<unsigned int, 4> getTxPDO_COB_ID() const;
    
    /// getRxPDO_COB_ID to expose all of this COB_ID addresses
    /// \return array containing all 4 TxPDO COB_ID
    std::array<unsigned int, 4> getRxPDO_COB_ID() const;
    
    /// Ctor for CANopenNode
    /// \param address node address
    /// \param txPDOarray configuration of TXPDO
    /// \param rxPDOarray configuration of RXPDO
    CANopenNode(uint8_t address);
    
    /// Copy Ctor should not be used!
    /// \param otherNode meh
    CANopenNode(const CANopenNode& otherNode) = delete;
    
    /// Getter for address
    /// \return node address
    uint8_t getAddress() const;
    
    /// update PDO values
    /// \param frame CANframe received from CAN bus (copied)
    //void update(CANframe frame);
    
    /// update PDO values
    /// \param frame CANframe received from CAN bus (moved)
    void update(CANframe&& frame);
    
    /// function that indicates if there's something new
    /// \return bool true if there's something new
    //TODO maybe useless?
    bool hasNews();
    
    /// printable class
    /// \return string with informations
    std::stringstream allPDOtoString();
    
    /// TODO documentation
    /// \param frame
    virtual void updateSDOobject(const CANframe& frame) = 0;
    
    CANframe createSDO(const bool &doWrite, const DictionaryEntry& entry, const int &value);
    
private:
    int dummy;
    
protected:
    ///CANopen network address of this node
    uint8_t _address;
    ///In which mode this node is
    CANopenNodeMode _mode;
    /// stores the way that TxPDO are setup. All that regards TxPDO understanding must be done with it
    std::array<PDOmessage, 4> _txPDOarrayInterface;
    /// stores the way that RxPDO are setup. All that regards RxPDO understanding must be done with it
    std::array<PDOmessage, 4> _rxPDOarrayInterface;
    
    /// all the updated values for TxPDO1, TxPDO2, TxPDO3, TxPDO4 objects
    std::array<std::vector<std::reference_wrapper<int>>, 4> _txPDOvalues;
    /// all the updated values for RxPDO1, RxPDO2, RxPDO3, RxPDO4 objects
    std::array<std::vector<std::reference_wrapper<int>>, 4> _rxPDOvalues;
    
    ///TODO: useful?
    bool _hasNews = true;
    
    ///key is index and subindex this way: (index << 8)|(subindex). In other words, there is 2 unused bit to the left of index, then index, then subindex.
    ObjectsDictionary _dictionaryByIndexSubindex;
    
    bool _isConfigured;
    
    void configurePDOs(std::array<PDOmessage, 4> _txPDOarrayInterface, std::array<PDOmessage, 4> _rxPDOarrayInterface);
    
};

#endif //PASSIVITY_PROJECT_CANOPENNODE_H
