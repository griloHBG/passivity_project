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
#include "NodePDO.h"

class CANopenNode
{
public:
    //bool Transition(CANopenNodeEvent event);
    /// getTxPDO_COB_ID to expose all of this COB_ID addresses
    /// \return array containing all 4 TxPDO COB_ID
    std::array<unsigned int, 4> getTxPDO_COB_ID() const;
    
    /// getRxPDO_COB_ID to expose all of this COB_ID addresses
    /// \return array containing all 4 TxPDO COB_ID
    std::array<unsigned int, 4> getRxPDO_COB_ID() const;
    
    CANopenNode(uint8_t address, std::array<NodePDO, 4>&& txPdOarray, std::array<NodePDO, 4>&& rxPdOarray);
    
    uint8_t getAddress() const;
    
    CANopenNode(const CANopenNode& otherNode);
    
    void update(CANframe frame);
    void update(CANframe&& frame);
    
    bool hasNews();
    
    std::stringstream PDOproperties();
    
private:
    uint8_t _address;
    CANopenNodeMode _mode;
    /// stores the way that TxPDO are setup. All that regards TxPDO understanding must be done with it
    std::array<NodePDO, 4> _txPDOarrayInterface;
    /// stores the way that RxPDO are setup. All that regards RxPDO understanding must be done with it
    std::array<NodePDO, 4> _rxPDOarrayInterface;
    
    /// all the updated values for TxPDO1, TxPDO2, TxPDO3, TxPDO4 objects
    std::array<std::vector<int>, 4> _txPDOvalues;
    /// all the updated values for RxPDO1, RxPDO2, RxPDO3, RxPDO4 objects
    std::array<std::vector<int>, 4> _rxPDOvalues;
    
    ///TODO: useful?
    bool _hasNews = true;
};


#endif //PASSIVITY_PROJECT_CANOPENNODE_H
