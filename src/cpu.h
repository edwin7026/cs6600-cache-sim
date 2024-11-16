/**
 * @file cpu.h
 * @details This file contains the class declaration for a cpu that initiates memory accesses
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#ifndef CPU_H
#define CPU_H

// standard includes
#include <string>
#include <fstream>
#include <sstream>

// local includes
#include <module.h>
#include <common.h>

/**
 * @details This class mimics a CPU issuing memory requests to the next memory module
 */
class cpu : public module
{
    private:
       std::string _trace_file_path;
    
    public:

        /**
         * @details Constructor that takes in path of the trace file and logger
         */
        cpu(const std::string& path, const logger& log);

        /**
         * @details Sequencer that sequences memory accesses
         */
        void sequencer();

        /**
         * @details Override get_frm_next to model memory instruction commit
         */
        void get_frm_next();
};

#endif // CPU_H