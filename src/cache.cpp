/**
 * @file cache.cpp
 * @details This file contains class definition of generic cache
 * @author Edwin Joy edwin7026@gmail.com
 **/

#include "cache.h"

cache::cache()
{
    // TODO decide on default values
}

cache::cache(const std::string& name, const unsigned& size, const unsigned& assoc, const unsigned& blocksize, 
    const unsigned& num_victim_blocks,
    const logger& log_obj,
    const repl_policy_enum& repl_policy
    ) :
    module(name, log_obj),
    _size(size),
    _assoc(assoc),
    _blocksize(blocksize),
    _repl_policy(repl_policy),
    _v_cache_states(size / blocksize / assoc, std::vector<cache_line_states>(assoc)),
    _v_victim_cache(num_victim_blocks)
{
    // construct module

    // cache parameters
    _size = size;
    _assoc = assoc;
    _blocksize = blocksize;
    _repl_policy = repl_policy;

    // get size parameters
    _num_blocks = _size / _blocksize;
    _num_sets = _num_blocks / _assoc;
    _repl_policy = repl_policy;

    // logging construction
    log.log(this, verbose::DEBUG, "Constructing " + name + " Cache");
}

// generic interface functions

void cache::get_frm_prev()
{
    if (ifc_prev != nullptr)
    {
        log.log(this, verbose::DEBUG, "Received request packet " + req_ptr -> get_msg_str());

        // gather the tag and set out of the address
        unsigned address = req_ptr -> addr;
        unsigned block_bit_size = (int) std::log2(_num_blocks);
        unsigned set_index_bit_size = (int) std::log2(_num_sets);
        
        unsigned set = (address >> block_bit_size) & ( (int) std::pow(2, set_index_bit_size) - 1);
        unsigned tag = address >> (block_bit_size + set_index_bit_size);

        log.log(this, verbose::DEBUG, "Set Index: 0x" + to_hex_str(set) + ", Tag: 0x" + to_hex_str(tag));

        // DEBUG
        // an always hit scenario

        // for hit scenario
        auto resp = new resp_msg(true, address);

        resp_ptr = resp;

        put_to_prev(resp);

        delete resp;
    }
}

void cache::put_next()
{


}

void cache::get_next(unsigned addr)
{

}


void cache::put_prev()
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
