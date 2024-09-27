/**
 * @file common.h
 * @details This file contains class definition of some common functions and classes
 * @author Edwin Joy edwin7026@gmail.com
 **/

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <iostream>
#include <sstream>

#include <base.h>

/**
 * @details This enumerates various logging levels
 */
enum verbose{
    WARN,
    ERROR,
    FATAL,
    INFO,
    DEBUG,
};

/**
 * @details Function that converts integer to string hex
 */
const std::string to_hex_str(unsigned val);

/**
 * @details This class employs a logger function
 */
class logger : public base
{
    private:
        verbose log_lvl;
    public:

        /**
         * 
         * @details Default constructor setting default logging level to INFO
         */
        logger() : base("Logger")
        {
            // get the logger level
            log_lvl = verbose::INFO;
        }
        
        /**
         * @details Construct with a static logging level
         */
        logger(verbose level) : base("Logger")
        {
            log_lvl = level;
        }

        /**
         * @details Log a message
         */
        void log(base* base_ptr, verbose level, const std::string& msg)
        {
            // TODO
            // Should be a better way to do this 
            std::string lvl_str = "";
            switch(level){
                case(verbose::DEBUG) : lvl_str = "DEBUG"; break;
                case(verbose::INFO) : lvl_str = "INFO"; break;
                case(verbose::FATAL) : lvl_str = "FATAL"; break;
                case(verbose::ERROR) : lvl_str = "ERROR"; break;
                case(verbose::WARN) : lvl_str = "WARN"; break;
            }

            // print if the logging level specified is higher than static
            if (level <= log_lvl)
            {
                std::cout << lvl_str << ": " << base_ptr->get_name() << " :: " << msg << std::endl;
            }
        }
};

#endif // COMMON_H