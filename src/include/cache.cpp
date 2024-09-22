/**
 * @file cache.cpp
 * @details This file contains class definition of generic cache
 * @author Edwin Joy edwin7026@gmail.com
 **/

#pragma once

#include "cache.h"

cache::cache()
{
    // TODO decide on default values
}

cache::cache(const unsigned& size, const unsigned& assoc, const unsigned& blocksize, 
    const unsigned& num_victim_blocks, 
    const repl_policy_enum& repl_policy = repl_policy_enum::LRU) :
    _size(size),
    _assoc(assoc),
    _blocksize(blocksize),
    _repl_policy(repl_policy),
    _v_cache_states(size / blocksize / assoc, std::vector<cache_line_states>(assoc)),
    _v_cache(size / blocksize / assoc, std::vector<cache_line>(assoc, cache_line(blocksize))),
    _v_victim_cache(num_victim_blocks, cache_line(blocksize))
{
    // cache parameters
    _size = size;
    _assoc = assoc;
    _blocksize = blocksize;
    _repl_policy = repl_policy;

    // get size parameters
    _num_blocks = _size / _blocksize;
    _num_sets = _num_blocks / _assoc;
    _repl_policy = repl_policy;
}

// generic interface functions

void cache::put_next()
{

}

void cache::get_next(unsigned addr)
{

}


void cache::put_prev()
{

}

void cache::get_prev()
{

}

// cache operations

void cache::replace()
{

}

void cache::write()
{

}

// cache destructor

cache::~cache()
{

}
