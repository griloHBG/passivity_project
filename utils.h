//
// Created by grilo on 04/02/2020.
//

#ifndef PASSIVITY_PROJECT_UTILS_H
#define PASSIVITY_PROJECT_UTILS_H

//Needs C++17 for std::string_view
//how to use:
// xablau values;
// std::cout << type_name<decltype(values)>() << std::endl;

#include <string_view>

template <typename T>
constexpr auto type_name()
{
    std::string_view name, prefix, suffix;
#ifdef __clang__
    name = __PRETTY_FUNCTION__;
    prefix = "auto type_name() [T = ";
    suffix = "]";
#elif defined(__GNUC__)
    name = __PRETTY_FUNCTION__;
    prefix = "constexpr auto type_name() [with T = ";
    suffix = "]";
#elif defined(_MSC_VER)
    name = __FUNCSIG__;
    prefix = "auto __cdecl type_name<";
    suffix = ">(void)";
#endif
    name.remove_prefix(prefix.size());
    name.remove_suffix(suffix.size());
    return name;
}

#endif //PASSIVITY_PROJECT_UTILS_H
