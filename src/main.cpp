#include <iostream>

#include <cache.h>

#include "utils.h"

int main(int argc, char* argv[]) 
{
    // parser to parse arguments to simulator
    
    // l1 parameters
    unsigned l1_cache_size = 1024;
    unsigned l1_cache_assoc = 2;
    unsigned l1_cache_block_size = 8;
    unsigned l1_cache_num_victim_blocks = 2;

    // construct a logger
    logger log(verbose::DEBUG);

    cache l1_cache(
        "L1",
        l1_cache_size,
        l1_cache_assoc,
        l1_cache_block_size,
        l1_cache_num_victim_blocks,
        log,
        repl_policy_enum::LRU);
    
    cache l2_cache(
        "L2",
        l1_cache_size,
        l1_cache_assoc,
        l1_cache_block_size,
        l1_cache_num_victim_blocks,
        log,
        repl_policy_enum::LRU);

    l1_cache.mk_next_connection(&l2_cache);

    return 0;
}