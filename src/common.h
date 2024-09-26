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
 * @details Enumerates different memory operations
 */
enum OP_TYPE
{
    LOAD,
    STORE,
};

/**
 * @details Function that converts integer to string hex
 */
const std::string to_hex_str(unsigned val);

/**
 * @details struct embodying a request packet
 */
struct mem_req
{
    OP_TYPE req_op_type;
    unsigned addr;

    // iniialize values
    mem_req() : req_op_type(OP_TYPE::LOAD), addr(0) {}
    mem_req(OP_TYPE op, unsigned addr) : req_op_type(op), addr(addr) {}

    /**
     * @details Function that elaborates this request packet as a string
     */
    const std::string get_msg_str()
    {
        return "{OP: " + std::to_string(req_op_type) + ", ADDR: " + to_hex_str(addr) + "}";
    };
};

/**
 * @details struch embodying response from a lower level
 */
struct resp_msg
{
    bool ready;
    unsigned addr;

    // construct with address
    resp_msg(bool rdy, unsigned addr) : ready(rdy), addr(addr) {}

    

    /**
     * @details Function the elaborates this request packet
     */
    const std::string get_msg_str()
    {
        return "{RDY: " + std::to_string(ready) + ", ADDR: " + to_hex_str(addr) + "}";
    }
};

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