/**
 * @file module.h
 * @details This file constains class declaration for module class
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#ifndef MODULE_H
#define MODULE_H

// standard includes
#include <string>

// local includes
#include <common.h>
#include <message.h>

/**
 * @details Generic module class
 */
class module : public base
{
    protected:
        // connections
        module* ifc_prev;        // towards core
        module* ifc_next;        // towards memory

        // get and put storage
        mem_req* req_ptr;
        resp_msg* resp_ptr;

        // logger instance
        logger log;
    public:
        /**
         * @details default constructor
         */
        module() : base("") {}

        /**
         * @details constructor that names the module
         */
        module(const std::string& name, const logger& log) : base(name), log(log) {
            ifc_prev = nullptr;
            ifc_next = nullptr;
        }

        // hierarchy constructor

        /**
         * @details make connection towards memory
         */
        void mk_next_connection(module* ifc)
        {
            if (ifc_next == nullptr)
            {   
                ifc_next = ifc;
                if (ifc != nullptr) {
                    log.log(this, verbose::DEBUG, "Connecting " + ifc->get_name() + " as next level");
                    ifc->mk_prev_connection(this);
                } 
            }
            else {
                log.log(this, verbose::DEBUG, "Connection to next level already made! Refusing connection");
            }
        }

        /**
         * @details make connection towards core
         */
        void mk_prev_connection(module* ifc)
        {
            if (ifc_prev == nullptr)
            {
                ifc_prev = ifc;
                if (ifc != nullptr) {
                    log.log(this, verbose::DEBUG, "Connecting " + ifc->get_name() + " as previous level");
                }
            }
            else {
                log.log(this, verbose::DEBUG, "Connection to previous level already made! Refusing connection");
            }
        }

        /**
        * @details get request pointer
        */
        mem_req* get_req() {
            return req_ptr;
        }


        // putters and getter interfaces

        /**
         * @details 
         */
        virtual void put_to_next(mem_req* req)
        {
            if (ifc_next != nullptr)
            {
                log.log(this, verbose::DEBUG, "Sending request packet " +  req->get_msg_str() + " --> " + ifc_next->get_name());

                // push message ptr to next level
                ifc_next -> req_ptr = req;

                // evaluate get method for next level
                ifc_next -> get_frm_prev();

                // pop off the message
                ifc_next -> req_ptr = nullptr;
            }
        }

        virtual void get_frm_next()
        {
            if (ifc_next != nullptr) {
                ifc_next -> put_to_prev(resp_ptr);
            }
        }
        
        virtual void put_to_prev(resp_msg* resp)
        {
            if (ifc_prev != nullptr)
            {
                
                log.log(this, verbose::DEBUG, "Sending response packet " + resp->get_msg_str() + " --> " + ifc_prev->get_name());

                // push resp message to previous level
                ifc_prev -> resp_ptr = resp;

                // evaluate get method for previous level
                ifc_prev -> get_frm_next();

                // pop off the message
                ifc_prev -> req_ptr = nullptr;
            }
        }

        virtual void get_frm_prev()
        {
            if (ifc_prev != nullptr) {
                ifc_prev -> put_to_next(req_ptr);
            }
        }

        /**
         * @details destructor
         */
        virtual ~module() = default;
};

#endif // MODULE_H