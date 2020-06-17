//
// Created by grilo on 04/02/2020.
//

#ifndef PASSIVITY_PROJECT_CANOPENUTILS_H
#define PASSIVITY_PROJECT_CANOPENUTILS_H

//Needs C++17 for std::string_view
//how to use:
// xablau values;
// std::cout << type_name<decltype(values)>() << std::endl;

#include <string_view>
#include <linux/can.h>
#include <sstream>
#include <iomanip>
#include "CANopenTypes.h"
/*
template <typename T>
constexpr auto type_name();
*/
std::string printCANframe(const can_frame& frame);

CANframe createSDO(const uint8_t address, const bool &doWrite, const IndexSubindex& indexSubindex, const CANopenDataType &dataType, const int &value);


unsigned int createCOBID(PDOType pdoType, uint8_t pdoNumber, uint8_t nodeAddress);

#endif //PASSIVITY_PROJECT_CANOPENUTILS_H
