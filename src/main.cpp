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
    unsigned l1_cache_size = 1024;
    unsigned l1_cache_assoc = 2;
    unsigned l1_cache_block_size = 16;
    unsigned l1_cache_num_victim_blocks = 0;

    // l2 parameters
    unsigned l2_cache_size = 8192;
    unsigned l2_cache_assoc = 4;
    unsigned l2_cache_block_size = 16;
    unsigned l2_cache_num_victim_blocks = 0;

    // construct a logger
    logger log(verbose::DEBUG);

    // initialize performance counters
    perf_counters::cache_counters hpm_counters_l1;
    perf_counters::cache_counters hpm_counters_l2; 
    
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
        &hpm_counters_l1);
    // attach performance counter
    hpm_counters_l1.attach_cache(&l1_cache);
    
    //l2
    cache l2_cache(
        "L2",
        l2_cache_size,
        l2_cache_assoc,
        l2_cache_block_size,
        l2_cache_num_victim_blocks,
        log,
        &hpm_counters_l2);
    // attach performance counter
    hpm_counters_l2.attach_cache(&l2_cache);
    
    // memory test
    main_memory main_mem(log);

    // make connections
    CPU.mk_next_connection(&l1_cache);
    l1_cache.mk_next_connection(&l2_cache);
    l2_cache.mk_next_connection(&main_mem);

    // start the sequencer
    CPU.sequencer();

    l1_cache.print_cache_content();
    l2_cache.print_cache_content();

    hpm_counters_l1.print();
    hpm_counters_l2.print();

    return 0;
}