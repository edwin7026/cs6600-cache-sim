/**
 * @file main_memory.h
 * @details This files constains the main memory model
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#include <module.h>

#ifndef MAIN_MEM_H
#define MAIN_MEM_H


class main_memory : public module
{
    private:
        unsigned _mem_acc_addr;
    public:

        /**
         * @details constructor that 
         */
        main_memory(logger log_obj) : module("Memory", log_obj)
        {
            log.log(this, verbose::DEBUG, "Constructing Main Memory");
            mk_next_connection(nullptr);
        }

        /**
         * @details Override get_frm_prev to model a always memory hit and return a response
         */
        void get_frm_prev()
        {
            if (ifc_prev != nullptr)
            {
                
                log.log(this, verbose::DEBUG, "Received request packet " +  ifc_prev -> get_req() -> get_msg_str());

                _mem_acc_addr = req_ptr -> addr;

                if (req_ptr -> req_op_type == OP_TYPE::LOAD)
                {
                    auto resp = new resp_msg(true, _mem_acc_addr);

                    // register this reponse
                    resp_ptr = resp;

                    put_to_prev(resp);
                
                    delete resp;
                } else {
                    // TODO do nothing

                    log.log(this, verbose::DEBUG, "Storing..");
                }
            }
        }

};

#endif // MAIN_MEM_H