#include <iostream>

#include <cpu.h>
#include <cache.h>
#include <main_memory.h>
#include <common.h>
#include <perf_counters.h>

int main(int argc, char* argv[]) 
{
    // l1 parameters
    unsigned l1_cache_size = 1024;
    unsigned l1_cache_assoc = 2;
    unsigned l1_cache_block_size = 16;
    unsigned l1_cache_num_victim_blocks = 0;

    // l2 parameters
    unsigned l2_cache_size = 0;
    unsigned l2_cache_assoc = 0;

    std::string trace_file_path = "";

    // parse arguments
    l1_cache_size = std::stoul(argv[1]);
    l1_cache_assoc = std::stoul(argv[2]);
    l1_cache_block_size = std::stoul(argv[3]);
    l1_cache_num_victim_blocks = std::stoul(argv[4]);

    l2_cache_size = std::stoul(argv[5]);
    l2_cache_assoc = std::stoul(argv[6]);

    trace_file_path = argv[7];

    // inferred params
    unsigned l2_cache_block_size = 16;
    unsigned l2_cache_num_victim_blocks = 0;
    
    // construct a logger
    logger log(verbose::INFO);

    // initialize performance counters
    perf_counters::cache_counters hpm_counters_l1;
    
    // cpu test
    cpu CPU(
        trace_file_path,
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

    // memory test
    main_memory main_mem(log);

    // make connections
    CPU.mk_next_connection(&l1_cache);

    std::cout << "===== Simulator configuration =====" << std::endl;
    std::cout << " L1_SIZE:\t" << l1_cache_size << std::endl;
    std::cout << " L1_ASSOC:\t" << l2_cache_assoc << std::endl;
    std::cout << " L1_BLOCSIZE:\t" << l2_cache_block_size << std::endl;
    std::cout << " VC_NUM_BLOCKS:\t" << l1_cache_num_victim_blocks << std::endl;
    std::cout << " L2_SIZE:\t" << l2_cache_size << std::endl;
    std::cout << " L2_ASSOC:\t" << l2_cache_assoc << std::endl;
    std::cout << " trace_file:\t" << trace_file_path << std::endl << std::endl;

    if (l2_cache_size != 0)
    {
        // performance counter for L2
        perf_counters::cache_counters hpm_counters_l2; 
        
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

        // make rest of the connections
        l1_cache.mk_next_connection(&l2_cache);
        l2_cache.mk_next_connection(&main_mem);

        // start CPU sequencer
        CPU.sequencer();

        // print contents
        l1_cache.print();
        l2_cache.print();

        hpm_counters_l1.print();
        hpm_counters_l2.print();
    }
    else
    {
        // no L2, so connect to main memory
        l1_cache.mk_next_connection(&main_mem);

        // start CPU sequencer
        CPU.sequencer();

        l1_cache.print();
        hpm_counters_l1.print();
    }
    
    return 0;
}