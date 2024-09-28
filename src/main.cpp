#include <iostream>

#include <cpu.h>
#include <cache.h>
#include <main_memory.h>
#include <common.h>

int main(int argc, char* argv[]) 
{
    // parser to parse arguments to simulator
    
    // l1 parameters
    unsigned l1_cache_size = 256;
    unsigned l1_cache_assoc = 1;
    unsigned l1_cache_block_size = 16;
    unsigned l1_cache_num_victim_blocks = 1;

    // construct a logger
    logger log(verbose::DEBUG);
    
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
        log);

    // memory test
    main_memory main_mem(log);

    // make connections
    CPU.mk_next_connection(&l1_cache);
    l1_cache.mk_next_connection(&main_mem);

    // start the sequencer
    CPU.sequencer();

    return 0;
}