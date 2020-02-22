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
    /// Ctor of PDOpart
    /// \param index position of this part, inside of its PDO
    /// \param description descriptive string of this guy
    /// \param dataType type of this part
    PDOpart(int8_t index, const std::string &description, CANopenDataType dataType);
    
    /// return this explained in a string
    /// \return the explained string
    std::stringstream toString() const;
    
public:
    /// Getter for the index
    /// \return PDOpart's index inside the PDO
    int8_t getIndex() const;
    
    /// Getter for the description
    /// \return PDOpart's description
    std::string getDescription() const;
    
    /// Getter for the data type
    /// \return PDOpart's data type
    CANopenDataType getDataType() const;
    
    /// Getter for the size
    /// \return PDOpart's size
    uint8_t getDataTypeSize() const;
    
    /// Getter for the type name
    /// \return PDOpart's type name
    const std::string &getDataTypeName() const;

private:
    ///stores the type
    CANopenDataType _dataType;
    ///stores the size of the type
    uint8_t _dataTypeSize;
    ///stores the name of the data type
    std::string _dataTypeName;
    ///stores the index of this PDOpart inside of the PDO payload
    int8_t _index;
    ///stores the description
    std::string _description;
};

#endif //PASSIVITY_PROJECT_PDOPART_H
