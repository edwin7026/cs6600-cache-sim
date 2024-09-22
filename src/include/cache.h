/**
 * @file cache.h
 * @details This file contains class declaration of generic cache
 * @author Edwin Joy edwin7026@gmail.com
 **/

#pragma once

#include <vector>
#include <cstdio>

struct cache_line_states
{
    // status bits
    bool _valid;
    bool _dirty;
    unsigned _tag;

    // initialize values to 0 at construction
    cache_line_states() : _valid(0), _dirty(0), _tag(0) {}
};

struct cache_line
{
    std::vector<std::byte> line;
    cache_line(unsigned line_size) : line(line_size) {}
};

/**
 * @details enumeration of various replacement policies
 */
enum repl_policy_enum {
    LRU
};

class cache
{
    private:
        // cache parameters:
        unsigned _size;
        unsigned _assoc;
        unsigned _blocksize;

        // cache properties
        unsigned _num_blocks;
        unsigned _num_sets;

        // victim cache size
        unsigned _num_victim_blocks;

        repl_policy_enum _repl_policy;

        // cache states
        std::vector<std::vector<cache_line_states>> _v_cache_states;

        // cache memory
        std::vector<std::vector<cache_line>> _v_cache;

        // victim cache
        std::vector<cache_line> _v_victim_cache;

    public:

        // default constructor
        cache();

        // contruct a cache based on size, associativity and block size
        cache(const unsigned& size, const unsigned& assoc, const unsigned& blocksize, 
            const unsigned& num_victim_blocks,
            const repl_policy_enum& repl_policy = repl_policy_enum::LRU);

        // module interface definitions for inter-cache communication

        /**
         * @details This function places requests to lower level in hierarchy
         **/ 
        void put_next();

        /**
         * @details This function accepts requests from lower level in hierarchy
         **/
        void get_next(unsigned addr);

        /**
         * @details This function places request to the next higher level in the hierarchy
         */
        void put_prev();

        /**
         * @details This function accepts requests from the next higher level in the hierarchy
         */
        void get_prev();

        // cache operations
        
        /**
         * @details replace a particular line from the cachce
         */
        void replace();

        /**
         * @details write to cache
         */
        void write();

        /**
         * @details read from cache
         */
        void read();

        // CPU operations

        /**
         * @details implementation of the reset routine for caches
         */
        void reset();

        /**
         * @details destructor
         */
        ~cache()
        {

        }
};