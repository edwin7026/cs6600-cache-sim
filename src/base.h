#pragma once

#include <string>

#ifndef BASE_H
#define BASE_H

/**
 * @details base class for all classes
 */
class base
{
    protected:
        std::string name;
    public:
        base(const std::string& name) : name(name) {}

        const std::string& get_name(){
            return name;
        }
};

#endif // BASE_H