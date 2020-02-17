//
// Created by grilo on 11/02/2020.
//

#ifndef PASSIVITY_PROJECT_PDOPART_H
#define PASSIVITY_PROJECT_PDOPART_H

#include <string>
#include "CANopenTypes.h"

class PDOpart
{
public:
    PDOpart(int8_t index, const std::string &description, CANopenDataType dataType);
    std::stringstream toString() const;
    
private:
    int8_t _index;
    std::string _description;
public:
    int8_t getIndex() const;
    
    const std::string &getDescription() const;
    
    CANopenDataType getDataType() const;
    
    uint8_t getDataTypeSize() const;
    
    const std::string &getDataTypeName() const;

private:
    CANopenDataType _dataType;
    uint8_t _dataTypeSize;
    std::string _dataTypeName;
};

#endif //PASSIVITY_PROJECT_PDOPART_H
