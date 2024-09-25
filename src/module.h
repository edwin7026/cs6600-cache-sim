/**
 * @file module.h
 * @details This file constains class declaration for module class
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#pragma once

// standard includes
#include <string>

// local includes
#include "utils.h"

/**
 * @details Generic module class
 */
class module : public base
{
    protected:
        // connections
        module* ifc_prev;        // towards core
        module* ifc_next;        // towards memory

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
            log.log(this, verbose::DEBUG, "Connecting " + ifc->get_name() + " as next level");
            ifc_next = ifc;
        }

        /**
         * @details make connection towards core
         */
        void mk_prev_connection(module* ifc)
        {
            log.log(this, verbose::DEBUG, "Connecting " + ifc->get_name() + " as previous level");
            ifc_prev = ifc;
        }

        // putters and getter interfaces

        /**
         * @details 
         */
        virtual void put_to_next()
        {
            if (ifc_next != nullptr)
                ifc_next -> get_frm_prev();
        }

        virtual void get_frm_next()
        {
            if (ifc_next != nullptr)
                ifc_next -> put_to_prev();
        }
        
        virtual void put_to_prev()
        {
            if (ifc_prev != nullptr)
                ifc_prev -> get_frm_next();   
        }

        virtual void get_frm_prev()
        {
            if (ifc_prev != nullptr)
                ifc_prev -> put_to_next();
        }

        /**
         * @details destructor
         */
        virtual ~module() = default;
};