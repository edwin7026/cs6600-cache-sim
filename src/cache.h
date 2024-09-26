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

#include <module.h>

struct cache_line_states
{
    // status bits
    bool _valid;
    bool _dirty;
    unsigned _tag;
    unsigned _count;

    // initialize values to 0 at construction
    cache_line_states() : _valid(0), _dirty(0), _tag(0), _count(0) {}
};

/**
 * @details enumeration of various replacement policies
 */
enum repl_policy_enum {
    LRU
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

        // victim cache size
        unsigned _num_victim_blocks;

        repl_policy_enum _repl_policy;

        // cache states
        std::vector<std::vector<cache_line_states>> _v_cache_states;

        // victim cache
        std::vector<cache_line_states> _v_victim_cache;

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
            const repl_policy_enum& repl_policy = repl_policy_enum::LRU
        );

        /**
         * @details This function makes the connection with other modules
         */
        void make_connection(module* module_ifc_prev, module* module_ifc_next);
        
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
        void get_frm_prev();

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
        ~cache();
};

#endif // CACHE_H