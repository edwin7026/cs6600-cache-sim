/**
 * @file cache.h
 * @details This file contains class declaration of generic cache
 * @author Edwin Joy edwin7026@gmail.com
 **/

#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <cstdio>
#include <string>
#include <cmath>

#include <assert.h>

#include <module.h>
#include <perf_counters.h>

struct cache_line_states
{
    // status bits
    bool _valid;
    bool _dirty;
    unsigned _tag;
    unsigned _count;

    // initialize values to 0 at construction
    cache_line_states() : _valid(false), _dirty(false), _tag(0), _count(0) {}
};

class cache : public module
{
    private:
        // cache parameters:
        unsigned _size;
        unsigned _assoc;
        unsigned _blocksize;

        // cache properties
        unsigned _num_blocks;
        unsigned _num_sets;

        unsigned _block_bit_size;
        unsigned _set_index_bit_size;

        // cache replacement helper data members
        bool _is_repl_on;
        cache_line_states* _repl_line;

        // eviction mode
        bool _is_evict_on;

        // victim cache
        bool _is_victim_cache_en;
        unsigned _num_victim_blocks;

        // performance counter
        perf_counters::cache_counters* hpm_counter_ptr;

        // cache states
        std::vector<std::vector<cache_line_states>> _v_cache_states;

        // victim cache
        std::vector<cache_line_states> _v_victim_cache;

        // Private member functions

        /**
         * @details Get the first invalid line
         */
        cache_line_states* get_invalid_line(std::vector<cache_line_states>& set_content);

        /**
         * @details LRU update on replacement
         */
        void lru_repl_update(std::vector<cache_line_states>* set_content, cache_line_states& hit_line);

        /**
         * @details Update counters of a line based on a hit
         */
        void lru_hit_update(std::vector<cache_line_states>* set_content, cache_line_states& hit_line);

        /**
         * @details Get least recently used line
         */
        cache_line_states* get_lru_line(std::vector<cache_line_states>& set_content);

        /**
         * @details Function to swap lines between main cache and victim cache
         */
        void swap_cache_line(unsigned cache_set, cache_line_states* cache_line_ptr, cache_line_states* victim_line_ptr);
 
    public:

        // default constructor
        cache();

        /**
         * @details contruct a cache based on size, associativity, block size, number of victim cache blocks and
         * replacement policy
         * Also pass in logger usec
         **/
        cache(const std::string& name, const unsigned& size, const unsigned& assoc, const unsigned& blocksize, 
            const unsigned& num_victim_blocks,
            const logger& log,
            perf_counters::cache_counters* hpm_counter
        );

        /**
         * @details This function makes the connection with other modules
         */
        void make_connection(module* module_ifc_prev, module* module_ifc_next);
        
        // module interface definitions for inter-cache communication

        /**
         * @details This overriding function accepts requests from the previous level in the hierarchy
         */
        void get_frm_prev();

        /**
         * @details This overriding function takes response from the next level in hierarchy
         */
        void get_frm_next();

        // utility functions

        /**
         * @details get the tag for main cache
         */
        unsigned get_cache_tag(unsigned addr);

        /**
         * @details get the tag for victim cache
         */
        unsigned get_victim_tag(unsigned addr);

        /**
         * @details get the set number
         */
        unsigned get_set_num(unsigned addr);

        /**
         * @details This function prints cache contents
         */
        void print_cache_content();

        // CPU operations

        /**
         * @details implementation of the reset routine for caches
         */
        void reset();

        /**
         * @details destructor
         */
        ~cache();
};

#endif // CACHE_H