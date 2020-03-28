//
// Created by grilo on 25/02/2020.
//

#ifndef PASSIVITY_PROJECT_EPOSNODE_H
#define PASSIVITY_PROJECT_EPOSNODE_H

#include <iostream>
#include "CANopenTypes.h"
#include "CANopenNode.h"

class EPOSnode : public CANopenNode
{
public:
    EPOSnode(uint8_t address);
    
    void updateSDOobject(const CANframe &frame) override;
    
    CANframe setPositionActualValue(int32_t value);
};

#endif //PASSIVITY_PROJECT_EPOSNODE_H
