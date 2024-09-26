/**
 * @file common.cpp
 * @details contains definitions of class and functions
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#include <common.h>

const std::string to_hex_str(unsigned val)
{
    std::stringstream  ss;
    ss << std::hex << val;
    return ss.str();
}