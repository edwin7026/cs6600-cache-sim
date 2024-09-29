#include <iostream>

#include <cpu.h>
#include <cache.h>
#include <main_memory.h>
#include <common.h>
#include <perf_counters.h>

int main(int argc, char* argv[]) 
{
    // parser to parse arguments to simulator
    
    // l1 parameters
    unsigned l1_cache_size = 512;
    unsigned l1_cache_assoc = 2;
    unsigned l1_cache_block_size = 16;
    unsigned l1_cache_num_victim_blocks = 2;

    // l2 parameters
    unsigned l2_cache_size = 512;
    unsigned l2_cache_assoc = 2;
    unsigned l2_cache_block_size = 16;
    unsigned l2_cache_num_victim_blocks = 0;

    // construct a logger
    logger log(verbose::DEBUG);

    // initialize performance counters
    perf_counters hpm_counters; 
    
    // cpu test
    cpu CPU(
        "./trace.txt",
        log
    );
     
    // l1
    cache l1_cache(
        "L1",
        l1_cache_size,
        l1_cache_assoc,
        l1_cache_block_size,
        l1_cache_num_victim_blocks,
        log,
        &hpm_counters);
    
    //l2
    cache l2_cache(
        "L2",
        l2_cache_size,
        l2_cache_assoc,
        l2_cache_block_size,
        l2_cache_num_victim_blocks,
        log,
        &hpm_counters);
 
    // memory test
    main_memory main_mem(log);

    // make connections
    CPU.mk_next_connection(&l1_cache);
    //l1_cache.mk_next_connection(&l2_cache);
    l1_cache.mk_next_connection(&main_mem);

    // start the sequencer
    CPU.sequencer();

    l1_cache.print_cache_content();
    //l2_cache.print_cache_content();

    hpm_counters.print();

    return 0;
}