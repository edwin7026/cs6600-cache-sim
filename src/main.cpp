#include <iostream>
#include <iomanip>

#include <cpu.h>
#include <cache.h>
#include <main_memory.h>
#include <common.h>
#include <perf_counters.h>

#include <parse.h>

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

    // performance counter for L2
    perf_counters::cache_counters hpm_counters_l2;

    // memory test
    main_memory main_mem(log);

    // make connections
    CPU.mk_next_connection(&l1_cache);

    // CACTI results for l1
    float l1_avg_access_time;
    float l1_energy;
    float l1_area;

    int l1_cacti_res = get_cacti_results(l1_cache_size,
                        l1_cache_block_size,
                        l1_cache_assoc,
                        &l1_avg_access_time,
                        &l1_energy,
                        &l1_area);
    assert(l1_cacti_res == 0);

    float l1_vc_avg_access_time = 0.0f;
    float l1_vc_energy = 0.0f;
    float l1_vc_area = 0.0f;
    int l1_vc_cacti_res = 3;

    // if victim cache enabled
    if (l1_cache_num_victim_blocks != 0)
    {
        l1_vc_cacti_res = get_cacti_results(
            l1_cache_num_victim_blocks * l1_cache_block_size,
            l1_cache_num_victim_blocks,
            l1_cache_num_victim_blocks,
            &l1_vc_avg_access_time,
            &l1_vc_energy,
            &l1_vc_area
        );

        // if cacti throws an error
        if (l1_vc_cacti_res != 0)
        {
            l1_vc_avg_access_time = 0.2f;
        }
    }

    std::cout << "===== Simulator configuration =====" << std::endl;
    std::cout << " L1_SIZE:\t\t" << l1_cache_size << std::endl;
    std::cout << " L1_ASSOC:\t\t" << l1_cache_assoc << std::endl;
    std::cout << " L1_BLOCSIZE:\t\t" << l1_cache_block_size << std::endl;
    std::cout << " VC_NUM_BLOCKS:\t\t" << l1_cache_num_victim_blocks << std::endl;
    std::cout << " L2_SIZE:\t\t" << l2_cache_size << std::endl;
    std::cout << " L2_ASSOC:\t\t" << l2_cache_assoc << std::endl;
    std::cout << " trace_file:\t\t" << trace_file_path << std::endl << std::endl;

    // for l2
    float l2_avg_access_time = 0.0f;
    float l2_energy = 0.0f;
    float l2_area = 0.0f;

    if (l2_cache_size != 0)
    {
        // CACTI results for l2


        int l2_cacti_res = get_cacti_results(l2_cache_size,
                            l2_cache_block_size,
                            l2_cache_assoc,
                            &l2_avg_access_time,
                            &l2_energy,
                            &l2_area);
        assert(l2_cacti_res == 0);
        
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
    }
    else
    {
        // no L2, so connect to main memory
        l1_cache.mk_next_connection(&main_mem);

        // start CPU sequencer
        CPU.sequencer();

        // print contents
        l1_cache.print();
    }

    float l1_swap_request_rate = 0.0f;
    float l1_vc_miss_rate = 0.0f;
    float l2_miss_rate = 0.0f;

    l1_vc_miss_rate = (hpm_counters_l1.read_misses + hpm_counters_l1.write_misses - hpm_counters_l1.num_swaps) / (float)(hpm_counters_l1.num_reads + hpm_counters_l1.num_writes);
 
    if (l2_cache_size != 0) {
        l2_miss_rate = hpm_counters_l2.read_misses / (float) hpm_counters_l2.num_reads;
    }

    // compute swap rate 
    if (l1_cache_num_victim_blocks != 0) {
        l1_swap_request_rate = hpm_counters_l1.num_swap_req / (float) (hpm_counters_l1.num_reads + hpm_counters_l1.num_writes);
    }

    // print simulation results
    std::cout << "===== Simulation results (raw) =====" << std::endl;
    std::cout << " a. number of L1 reads:\t" << hpm_counters_l1.num_reads << std::endl;
    std::cout << " b. number of L1 read misses:\t" << hpm_counters_l1.read_misses << std::endl;
    std::cout << " c. number of L1 writes:\t" << hpm_counters_l1.num_writes << std::endl;
    std::cout << " d. number of L1 write misses:\t" << hpm_counters_l1.write_misses << std::endl;
    std::cout << " e. number of swap requests:\t" << hpm_counters_l1.num_swap_req << std::endl;
    std::cout << " f. swap request rate:\t" << std::setprecision(4) <<  l1_swap_request_rate << std::endl;
    std::cout << " g. number of swaps:\t" << hpm_counters_l1.num_swaps << std::endl;
    std::cout << " h. combined L1+VC miss rate:\t" << std::setprecision(4) << l1_vc_miss_rate << std::endl;
    std::cout << " i. number of writebacks from L1/VC:\t" << hpm_counters_l1.num_writebacks << std::endl;
    std::cout << " j. number of L2 reads:\t" << hpm_counters_l2.num_reads << std::endl;
    std::cout << " k. number of L2 read misses:\t" << hpm_counters_l2.read_misses << std::endl;
    std::cout << " l. number of L2 writes:\t" << hpm_counters_l2.num_writes << std::endl;
    std::cout << " m. number of L2 write misses:\t" << hpm_counters_l2.write_misses << std::endl;
    std::cout << " n. L2 miss rate:\t" << std::setprecision(4) << l2_miss_rate << std::endl;
    std::cout << " o. number of writebacks from L2:\t" << hpm_counters_l2.num_writebacks << std::endl;
    std::cout << " p. total memory traffic:\t" << main_mem.mem_access << std::endl;

    // performance analysis
    double average_acc_time = 0.0f;
    double e_delay_prod = 0.0f;
    double total_area = 0.0f;
    double total_acc_time = 0.0f;
    double mem_energy = 0.05f;
    double mem_miss_penalty = 20.0f + l1_cache_block_size / 16.0f;
    double l1_miss_penalty = 0.0f;

    // if only l1-mem
    average_acc_time = l1_avg_access_time + l1_vc_miss_rate * l1_miss_penalty;

    // get l1 mis penalty
    if (l2_cache_size == 0) {
        l1_miss_penalty = mem_miss_penalty;
    } else {
        l1_miss_penalty = l2_avg_access_time + l2_miss_rate * mem_miss_penalty;
    }

    // get total area
    total_area = l1_area + l2_area + l1_vc_area;

    // average acceess time
    average_acc_time = l1_avg_access_time + l1_vc_miss_rate * l1_miss_penalty + l1_swap_request_rate * l1_vc_avg_access_time;
    
    // total access time
    total_acc_time = static_cast<double>(l1_avg_access_time) + (hpm_counters_l1.read_misses + hpm_counters_l1.write_misses - hpm_counters_l1.num_swaps) * static_cast<double>(l1_miss_penalty) \
                            + hpm_counters_l1.num_swap_req * static_cast<double>(l1_vc_avg_access_time);

    e_delay_prod = (hpm_counters_l1.num_reads + hpm_counters_l1.num_writes + hpm_counters_l1.read_misses + hpm_counters_l1.write_misses) * static_cast<double>(l1_energy);
    if (l1_cache_num_victim_blocks != 0)
        e_delay_prod += (2*hpm_counters_l1.num_swaps) * static_cast<double>(l1_vc_energy);
    if (l2_cache_size != 0) {
        e_delay_prod += (hpm_counters_l2.read_misses + hpm_counters_l2.write_misses + hpm_counters_l2.num_reads + hpm_counters_l2.num_reads) * static_cast<double>(l2_energy);
        e_delay_prod += (hpm_counters_l2.read_misses + hpm_counters_l2.write_misses + hpm_counters_l2.num_writebacks) * static_cast<double>(mem_energy);
    } else {
        e_delay_prod += (hpm_counters_l1.read_misses + hpm_counters_l1.write_misses - hpm_counters_l1.num_swaps + hpm_counters_l1.num_writebacks) * static_cast<double>(mem_energy);
    }
    e_delay_prod = e_delay_prod * total_acc_time * (hpm_counters_l1.num_reads + hpm_counters_l1.num_writes);

    // print
    std::cout << std::endl << "===== Simulation results (performance) =====" << std::endl;
    std::cout << " 1. average access time:\t" << std::setprecision(5) << average_acc_time << std::endl;
    std::cout << " 2. energy-delay product:\t" << std::setprecision(14) <<  e_delay_prod << std::endl;
    std::cout << " 3. total area:\t" << std::setprecision(4) << total_area << std::endl;
        
    return 0;
}