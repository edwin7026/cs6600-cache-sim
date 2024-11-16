/**
 * @file message.h
 * @details This file contains the classes pertaining to request and response messages
 * @author Edwin Joy <edwin7026@gmail.com>
 */

// standard include
#include <string>

// local include
#include <common.h>

/**
 * @details Enumerates different memory operations
 */
enum OP_TYPE
{
    LOAD,
    STORE,
};

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
        return "{OP: " + std::to_string(req_op_type) + ", ADDR: 0x" + to_hex_str(addr) + "}";
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
        return "{RDY: " + std::to_string(ready) + ", ADDR: 0x" + to_hex_str(addr) + "}";
    }
};