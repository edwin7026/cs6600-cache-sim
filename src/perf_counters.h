/**
 * @file perf_counters.h
 * @details This file contains struct definition for performance counters
 * @author Edwin Joy <edwin7026@gmail.com>
 */

#ifndef PERF_COUNTERS_H
#define PERF_COUNTERS_H

#include <iostream>

struct perf_counters {
    unsigned num_reads;
    unsigned read_misses;
    unsigned num_writes;
    unsigned write_misses;

    unsigned num_swap_req;
    float swap_req_rate;

    float cache_vc_miss_rate;

    unsigned num_writebacks;

    unsigned mem_traffic;

    perf_counters()
    {
        // reset counters
        num_reads = 0;
        read_misses = 0;
        num_writes = 0;
        write_misses = 0;
        num_swap_req = 0;
        swap_req_rate = 0;
        cache_vc_miss_rate = 0;
        num_writebacks = 0;
        mem_traffic = 0;
    }

    void print()
    {
        std::cout << "Number of L1 reads: " << num_reads << std::endl;
        std::cout << "Number of L1 writes: " << num_writes << std::endl;
        std::cout << "Number of L1 read misses: " << read_misses << std::endl;
        std::cout << "Number of L1 write misses: " << write_misses << std::endl;
    }

};

#endif // PERF_COUNTERS_H