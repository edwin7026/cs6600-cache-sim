/**
 * @file cpu.cpp
 * @details This file contains definitions for class members
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#include "cpu.h"

cpu::cpu(const std::string &path, const logger& log_obj) : module("Core", log_obj)
{
    log.log(this, verbose::DEBUG, "Constructing CPU");

    // CPU issues all requests to the memory hierarchy
    this->mk_prev_connection(nullptr);

    // trace file path
    _trace_file_path = path;
}

void cpu::sequencer()
{
    log.log(this, verbose::INFO, "Reading trace file: " + _trace_file_path);
    
    std::ifstream stream(_trace_file_path);

    // temp variables 
    std::string line;
    std::string opr;

    // line counter
    unsigned count = 0;

    // error flag
    bool err = false;

    if (stream.is_open())
    {
        while (getline(stream, line))
        {
            mem_req* req_msg = new mem_req;

            log.log(this, verbose::DEBUG, "Reading line: " + line);

            count = count + 1;

            std::stringstream ss(line);

            // get opr type
            ss >> opr;
            if (opr == "r") {
                req_msg->req_op_type = OP_TYPE::LOAD;
            }
            else if (opr == "w") {
                req_msg->req_op_type = OP_TYPE::STORE;
            }
            else 
            {
                // log an error
                log.log(this, verbose::FATAL, _trace_file_path + ": Invalid request format at line " + std::to_string(count));
                err = true;
                break;
            }

            // get address
            ss >> opr;
            try {
               req_msg->addr = std::stoul(opr, nullptr, 16);
            }
            catch (const std::invalid_argument&) {
               log.log(this, verbose::FATAL, _trace_file_path + ": Cannot convert address hex to int at line " + std::to_string(count));
               err = false;
               break;
            }

            // register this request
            req_ptr_next = req_msg;

            // send out a request through put next port
            put_to_next(req_msg);
            
            // dequeue the request
            delete req_msg;
        }
    }
    else {
        // file is not open yet
        log.log(this, verbose::FATAL, _trace_file_path + ": Unable to find file");
    }

    // execution is done
    if (err) {
        log.log(this, verbose::INFO, "Ending with errors");
    } else {
        log.log(this, verbose::INFO, "Execution completed!");
    }
}

void cpu::get_frm_next()
{
    log.log(this, verbose::DEBUG, "Committing " + req_ptr_next->get_msg_str());
}