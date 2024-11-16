/**
 * @file perf_counters.h
 * @details This file contains struct definition for performance counters
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#ifndef PERF_COUNTERS_H
#define PERF_COUNTERS_H

#include <iostream>
#include <module.h>

namespace perf_counters
{
    struct cache_counters 
    {
        module* cache_ptr;    

        unsigned num_reads;
        unsigned read_misses;
        unsigned num_writes;
        unsigned write_misses;
        unsigned num_swap_req;
        unsigned num_swaps;
        unsigned num_writebacks;

        cache_counters()
        {
            // reset counters
            num_reads = 0;
            read_misses = 0;
            num_writes = 0;
            write_misses = 0;
            num_swap_req = 0;
            num_writebacks = 0;
            cache_ptr = nullptr;
        }

        void attach_cache(module* ptr){
            cache_ptr = ptr;
        }

        void print()
        {
            std::cout << "Number of " << cache_ptr->get_name() << " reads: " << num_reads << std::endl;
            std::cout << "Number of " << cache_ptr->get_name() << " read misses: " << read_misses << std::endl;
            std::cout << "Number of " << cache_ptr->get_name() << " writes: " << num_writes << std::endl;
            std::cout << "Number of " << cache_ptr->get_name() << " write misses: " << write_misses << std::endl;
            std::cout << "Number of " << cache_ptr->get_name() << " swap requests: " << num_swap_req << std::endl;
            std::cout << "Number of swaps: " << num_swaps << std::endl;
        }
    };
}


#endif // PERF_COUNTERS_H