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
    _num_victim_blocks(num_victim_blocks),
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

    _block_bit_size = (int) std::log2(_num_blocks);
    _set_index_bit_size = (int) std::log2(_num_sets);
    _is_repl_on = false;

    // enable hardware for victim cache if number of blocks is greater than 0
    _is_victim_cache_en = false;
    if (num_victim_blocks > 0) {
        _is_victim_cache_en = true;
    }

    // logging construction
    log.log(this, verbose::DEBUG, "Constructed " + name + " Cache");
}

// generic interface functions

void cache::get_frm_prev()
{
    if (ifc_prev != nullptr)
    {
        log.log(this, verbose::DEBUG, "Received request packet " + req_ptr -> get_msg_str());

        // gather the tag and set out of the address
        unsigned address = req_ptr -> addr;
        
        unsigned req_set = (address >> _block_bit_size) & ( (int) std::pow(2, _set_index_bit_size) - 1);
        unsigned req_tag = address >> (_block_bit_size + _set_index_bit_size);
        unsigned req_tag_victim = address >> _block_bit_size;

        log.log(this, verbose::DEBUG, "Requested Set Index: 0x" + to_hex_str(req_set) + ", Tag: 0x" + to_hex_str(req_tag));

        // get set
        auto set_content = _v_cache_states[req_set];
        bool is_hit = false;

        // loop through lines for a tag match
        for (auto& line : set_content)
        {
            // tag matcing
            if (req_tag == line._tag && line._valid)
            {
                // Cache hit
                is_hit = true;
                log.log(this, verbose::DEBUG, "Cache Hit");

                if (req_ptr -> req_op_type == OP_TYPE::LOAD) 
                {
                    // nothing really to be done
                } 
                else if (req_ptr -> req_op_type == OP_TYPE::STORE)
                {
                    // set line to dirty
                    line._dirty = true;
                }

                // LRU stuffs happen here
                lru_hit_update(set_content, line);

                // create a response back previous level to move to the next request
                auto resp = new resp_msg(true, address);
                resp_ptr = resp;

                put_to_prev(resp);

                // deque the response message
                delete resp;

                // break from the loop
                break;
            }
            else
            {
                // TODO
            }
        }

        // handle a miss
        if (!is_hit)
        {
            log.log(this, verbose::DEBUG, "Cache miss!");

            // get invalid line to fetch the request to
            cache_line_states* inv_line_ptr = get_invalid_line(set_content);
            
            // is relacement needed
            if (inv_line_ptr == nullptr)
            {   
                // replacement needed due to conflict miss
                _is_repl_on = true;
                log.log(this, verbose::DEBUG, "Replacement needed!");
               
                // if victim cache exists, check through victim cache
                if (_is_victim_cache_en)
                {
                    bool is_victim_hit = false;
                    log.log(this, verbose::DEBUG, "Looking through victim cache");
                    for (auto& victim_line : _v_victim_cache)
                    {
                        if (req_tag_victim == victim_line._tag && victim_line._valid)
                        {
                            // hit in victim cache
                            log.log(this, verbose::DEBUG, "Victim cache hit!");
                            is_victim_hit = true;

                            // get least recently used line
                            cache_line_states* lru_line = get_lru_line(set_content);

                            // update tag appropriately in main and victim cache before putting in victim cache
                            lru_line->_tag = (lru_line->_tag << _set_index_bit_size) | req_set;
                            victim_line._tag = victim_line._tag >> _set_index_bit_size;

                            // swap data
                            std::swap(victim_line, *lru_line);

                            // Update LRU counters on hit in victim cache
                            lru_hit_update(_v_victim_cache, *lru_line);

                            // Update LRU counters in main cache
                            lru_repl_update(set_content, victim_line);

                            break;
                        }
                    }

                    if (!is_victim_hit)
                    {
                        // victim cache miss
                        log.log(this, verbose::DEBUG, "Victim cache miss! Requesting data from next level");

                        // request the next level to give the line
                        if (ifc_next != nullptr) {
                            put_to_next(req_ptr);
                        }
                    }
                }

                _is_repl_on = false;
            }
            else
            {
                // miss in l1 but there are more free ways
                log.log(this, verbose::DEBUG, "Requesting data from next level");

                // request next level to give the line
                if (ifc_next != nullptr) {
                    put_to_next(req_ptr);
                }
            }
        }

        // DEBUG
        // an always hit scenario

        // for hit scenario
        // auto resp = new resp_msg(true, address);

        // resp_ptr = resp;

        // put_to_prev(resp);

        // delete resp;
    }
}

void cache::get_frm_next()
{
    // getting data from next level could be for two reasons
    // 1. miss in main cache, miss in victim cache (needs replacement with victim cache)
    // 2. miss in main cache (does not need replacement with victim cache)

    // if replacement flag is set
    if (_is_repl_on)
    {

    }
}

// cache operations

cache_line_states* cache::get_invalid_line(std::vector<cache_line_states>& set_content)
{
    for (auto& line : set_content)
    {
        if (!line._valid) {
            return &line;
        }
    }

    std::cout << "KOOOKOOO" << std::endl;
    return nullptr;
}

void cache::lru_repl_update(const std::vector<cache_line_states>& set_content, cache_line_states& hit_line)
{
    // reset counter for new line
    hit_line._count = 0;

    // increment counters for all other lines
    for (auto other_line : set_content)
    {
        if (other_line._tag != hit_line._tag && other_line._valid)
        {
            if (other_line._count != _assoc) {
                other_line._count = other_line._count + 1;
            }
        }
    }
}

void cache::lru_hit_update(const std::vector<cache_line_states>& set_content, cache_line_states& hit_line)
{

    log.log(this, verbose::DEBUG, "Updating LRU counters");

    // get older counter
    unsigned old_count = hit_line._count;
    // reset counter for hit line
    hit_line._count = 0;
                
    // increment counters of other lines accordingly
    for (auto other_line : set_content)
    {
        if (other_line._tag != hit_line._tag && other_line._valid)
        {
            // incrememnt counters for all other lines with count less than old count of hit line
            if (other_line._count < old_count)
            {
                // handle saturation
                if (other_line._count != _assoc) {
                    other_line._count = other_line._count + 1;
                }
            }
        }
    }
}

cache_line_states* cache::get_lru_line(std::vector<cache_line_states>& set_content)
{
    unsigned max_count = 0;
    cache_line_states* lru_line_ptr;

    for (auto& line : set_content)
    {
        if (line._count > max_count && line._valid) {
            lru_line_ptr = &line;
        }
    }

    return lru_line_ptr;
}

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
