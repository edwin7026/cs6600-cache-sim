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
    perf_counters::cache_counters* hpm_counter) :
    module(name, log_obj),
    _size(size),
    _assoc(assoc),
    _blocksize(blocksize),
    _num_victim_blocks(num_victim_blocks),
    _v_cache_states(size / blocksize / assoc, std::vector<cache_line_states>(assoc)),
    _v_victim_cache(num_victim_blocks)
{
    // construct module

    // cache parameters
    _size = size;
    _assoc = assoc;
    _blocksize = blocksize;

    // get size parameters
    _num_blocks = _size / _blocksize;
    _num_sets = _num_blocks / _assoc;

    _block_bit_size = (int) std::log2(_blocksize);
    _set_index_bit_size = (int) std::log2(_num_sets);
    _is_repl_on = false;
    _repl_line = nullptr;
    _is_evict_on = false;

    hpm_counter_ptr = hpm_counter;

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

        //print_cache_content();

        log.log(this, verbose::DEBUG, "Received request packet " + req_ptr_prev -> get_msg_str());

        // increment request counter
        if (req_ptr_prev -> req_op_type == OP_TYPE::LOAD) {
            hpm_counter_ptr->num_reads++;
        }
        else {
            hpm_counter_ptr->num_writes++;
        }

        // gather the tag and set out of the address
        unsigned address = req_ptr_prev -> addr;

        // for tag and set matching 
        unsigned req_set = get_set_num(address);
        unsigned req_tag = get_cache_tag(address);
        unsigned req_tag_victim = get_victim_tag(address);

        log.log(this, verbose::DEBUG, "Requested Set Index: 0x" + to_hex_str(req_set) + ", Tag: 0x" + to_hex_str(req_tag));
        
        // DEBUG
        // std::getchar();

        // get set
        auto& set_content = _v_cache_states[req_set];
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

                if (req_ptr_prev -> req_op_type == OP_TYPE::LOAD) 
                {
                    // nothing really to be done
                } 
                else if (req_ptr_prev -> req_op_type == OP_TYPE::STORE)
                {
                    // set line to dirty
                    line._dirty = true;
                }

                // LRU stuffs happen here
                lru_hit_update(&set_content, line);

                // create a response back previous level to move to the next request
                auto resp = new resp_msg(true, address);
                resp_ptr_prev = resp;

                put_to_prev(resp_ptr_prev);

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

            // increment request counter
            if (req_ptr_prev -> req_op_type == OP_TYPE::LOAD) {
                hpm_counter_ptr->read_misses++;
            }
            else {
                hpm_counter_ptr->write_misses++;
            }

            // get invalid line to fetch the request to
            cache_line_states* inv_line_ptr = get_invalid_line(set_content);

            // Check victim cache

            // if victim cache exists and no space in set, check through victim cache
            if (_is_victim_cache_en && inv_line_ptr == nullptr)
            {
                bool is_victim_hit = false;
                log.log(this, verbose::DEBUG, "Looking through victim cache");
                for (auto victim_line = _v_victim_cache.begin(); victim_line != _v_victim_cache.end(); victim_line++)
                {
                    if (req_tag_victim == victim_line->_tag && victim_line->_valid)
                    {
                        // hit in victim cache
                        log.log(this, verbose::DEBUG, "Victim cache hit!");
                        is_victim_hit = true;

                        // get least recently used line
                        if (inv_line_ptr == nullptr) {

                            log.log(this, verbose::DEBUG, "Set full, initiating LRU replacement with victim cache");
                            
                            // no space in set. swap with the lru line in main cache
                            cache_line_states* lru_line = get_lru_line(set_content);
                            
                            // increment swap requests number
                            hpm_counter_ptr->num_swap_req++;

                            swap_cache_line(req_set, lru_line, &*victim_line);

                            // exchange count values
                            std::swap(victim_line->_count, lru_line->_count);
                            
                            // Update LRU counters on hit in victim cache
                            lru_hit_update(&_v_victim_cache, *victim_line);

                            // Update LRU counters in main cache
                            lru_repl_update(&set_content, *lru_line);
                        }
                        else 
                        {
                            // there are invalid lines, bring that line here and invalidate the one in victim cache
                            log.log(this, verbose::FATAL, "Non-full set should not have a victim cache hit!");
                            assert(false);
                        }

                        // done with loop
                        break;
                    }
                    else {
                        // TODO
                        // Track states for optimization
                    }
                }

                if (!is_victim_hit)
                {
                    // victim cache miss
                    log.log(this, verbose::DEBUG, "Victim cache miss!");

                    _repl_line = inv_line_ptr;

                    // set parameters accordingly to flag if replacement is needed 
                    if (inv_line_ptr == nullptr)
                    {   
                        log.log(this, verbose::DEBUG, "Set is full. Replacement needed!");

                        // flag set for replacement
                        _is_repl_on = true;
                    }
                    else {
                        _is_repl_on = false;
                    }

                    // request from next level
                    if (ifc_next != nullptr)
                    {
                        req_ptr_next = req_ptr_prev;
                        put_to_next(req_ptr_next);
                    } else {
                        log.log(this, verbose::FATAL, "No next level connection! Check connections");
                    }
                }
            }
            else
            {
                // No victim cache
                _repl_line = inv_line_ptr;

                if (inv_line_ptr == nullptr)
                {
                    log.log(this, verbose::DEBUG, "Set is full. Replacement needed!");
                    
                    // flag set for replacement
                    _is_repl_on = true;
                }
                else
                {
                    _is_repl_on = false;
                }

                // request from next level
                if (ifc_next != nullptr)
                {
                    // push request to next elvel
                    req_ptr_next = req_ptr_prev;
                    put_to_next(req_ptr_next);
                } else {
                    log.log(this, verbose::FATAL, "No next level connection! Check conncections");
                }
            }
        }
    }
}

void cache::get_frm_next()
{
    // getting data from next level could be for two reasons
    // 1. miss in main cache, miss in victim cache (needs replacement with victim cache)
    // 2. miss in main cache (does not need replacement with victim cache)

    log.log(this, verbose::DEBUG, "Received response packet " + resp_ptr_next->get_msg_str());

    unsigned resp_set = get_set_num(resp_ptr_next->addr);

    // if evict mode is on ignore the response
    if (_is_evict_on) {
        return;
    }

    // if replacement flag is set
    if (_is_repl_on)
    {
        if (_repl_line == nullptr)
        {
            // main cache set is full
            log.log(this, verbose::DEBUG, "Replacing line");

            // get lru line from main cache
            cache_line_states* cache_lru_line = get_lru_line(_v_cache_states[resp_set]);

            if (_is_victim_cache_en)
            {
                // check if victim cache is full
                cache_line_states* victim_inv_line = get_invalid_line(_v_victim_cache);

                if (victim_inv_line == nullptr)
                {
                    // victim cache full
                    log.log(this, verbose::DEBUG, "Victim cache full. Evicting");
                    _is_evict_on = true;

                    // evicting lru line from victim cache
                    // get lru line from victim cache
                    cache_line_states* victim_lru_line = get_lru_line(_v_victim_cache);

                    if (victim_lru_line -> _dirty)
                    {
                        
                        log.log(this, verbose::DEBUG, "Line is dirty. Evicting");

                        // create new request packet
                        mem_req* evict_req = new mem_req;

                        // write request to next level
                        evict_req->req_op_type = OP_TYPE::STORE;
                        evict_req->addr = victim_lru_line->_tag << _block_bit_size;

                        put_to_next(evict_req);

                        // retire the eviction request
                        delete evict_req;
                    }
                    else {
                        log.log(this, verbose::DEBUG, "Line is not dirty. Invalidating");
                    }

                    // invalidate the line in victim cache
                    victim_lru_line -> _valid = false;

                    // swap the lru line in main cache with evicted line
                    // and invalidate the line put in main cache

                    // swap lines
                    swap_cache_line(resp_set, cache_lru_line, victim_lru_line);
                    
                    // invalidate the line in cache
                    cache_lru_line -> _valid = false;

                    // update lru numbers in victim cache
                    lru_repl_update(&_v_victim_cache, *victim_lru_line);

                    // finally update the lru line with response
                    cache_lru_line ->_dirty = (bool) req_ptr_prev -> req_op_type;
                    cache_lru_line -> _tag = get_cache_tag(req_ptr_prev -> addr);
                    cache_lru_line -> _valid = true;
                    cache_lru_line -> _count = 0;
                
                    lru_repl_update(&_v_cache_states[resp_set], *cache_lru_line);

                    _is_evict_on = false;

                    if (ifc_prev != nullptr) {
                        resp_ptr_prev = resp_ptr_next;
                        put_to_prev(resp_ptr_prev);
                    }
                }
                else
                {
                    log.log(this, verbose::DEBUG, "Evicting line to victim cache");

                    swap_cache_line(resp_set, cache_lru_line, victim_inv_line);

                    // invalidate the line in cache
                    cache_lru_line -> _valid = false;

                    // update lru numbers in victim cache
                    lru_repl_update(&_v_victim_cache, *victim_inv_line);

                    log.log(this, verbose::DEBUG, "Allocating response line");

                    // fincallu update the lru cache line with response
                    cache_lru_line ->_dirty = (bool) req_ptr_prev -> req_op_type;
                    cache_lru_line -> _tag = get_cache_tag(req_ptr_prev -> addr);
                    cache_lru_line -> _valid = true;
                    cache_lru_line -> _count = 0;

                    // update LRU numbers affter miss 
                    lru_repl_update(&_v_cache_states[resp_set], *cache_lru_line);

                    // put the response back to previous level 
                    if (ifc_prev != nullptr) {
                        resp_ptr_prev = resp_ptr_next;
                        put_to_prev(resp_ptr_prev);
                    }
                }
            }
            else
            {
                _is_evict_on = true;

                if (cache_lru_line -> _dirty)
                {
                    log.log(this, verbose::DEBUG, "Line is dirty. Evicting");

                    // create new request packet
                    mem_req* evict_req = new mem_req;

                    // write request to next level
                    evict_req->req_op_type = OP_TYPE::STORE;
                    evict_req->addr = cache_lru_line->_tag << (_set_index_bit_size + _block_bit_size);

                    put_to_next(evict_req);

                    // retire the eviction request
                    delete evict_req;
                }
                else {
                    log.log(this, verbose::DEBUG, "Line is not dirty. Evicting");
                }

                cache_lru_line -> _dirty = (bool) req_ptr_prev -> req_op_type;
                cache_lru_line -> _tag = get_cache_tag(req_ptr_prev -> addr);
                cache_lru_line -> _valid = true;
                cache_lru_line -> _count = 0;

                lru_repl_update(&_v_cache_states[resp_set], *cache_lru_line);

                _is_evict_on = false;

                if (ifc_prev != nullptr) {
                    resp_ptr_prev = resp_ptr_next;
                    put_to_prev(resp_ptr_prev);
                }
            }
        }
        else
        {
            log.log(this, verbose::FATAL, "Replacement not needed for a non-full set");
        }
    }
    else 
    {
        
        log.log(this, verbose::DEBUG, "Writing response block to set: 0x" + to_hex_str(resp_set));

        // set valid
        _repl_line -> _valid = true;
        
        // update dirty or non-dirty
        if (req_ptr_prev -> req_op_type == OP_TYPE::LOAD) {
            _repl_line -> _dirty = false;
        } else {
            _repl_line -> _dirty = true;
        }
        
        // update tag
        _repl_line -> _tag = get_cache_tag(req_ptr_prev->addr);

        auto& set_content = _v_cache_states[resp_set];

        // handle miss scenario for LRU
        lru_repl_update(&set_content, *_repl_line);

        // put the response back to previous level 
        if (ifc_next != nullptr) {
            resp_ptr_prev = resp_ptr_next;
            put_to_prev(resp_ptr_prev);
        }
    }
}

// cache operations

cache_line_states* cache::get_invalid_line(std::vector<cache_line_states>& set_content)
{
    for (auto it=set_content.begin(); it != set_content.end(); it++)
    {
        if (!it->_valid) {
            return &*it;
        }
    }

    return nullptr;
}

void cache::lru_repl_update(std::vector<cache_line_states>* set_content, cache_line_states& hit_line)
{
    // reset counter for new line
    hit_line._count = 0;

    // increment counters for all other lines
    for (auto& other_line : *set_content)
    {
        if (other_line._tag != hit_line._tag && other_line._valid)
        {
            if (other_line._count != set_content->size()) {
                other_line._count = other_line._count + 1;
            }
        }
    }
}

void cache::lru_hit_update(std::vector<cache_line_states>* set_content, cache_line_states& hit_line)
{

    log.log(this, verbose::DEBUG, "Updating LRU counters");

    // get older counter
    unsigned old_count = hit_line._count;
    // reset counter for hit line
    hit_line._count = 0;
                
    // increment counters of other lines accordingly
    for (auto& other_line : *set_content)
    {
        if (other_line._tag != hit_line._tag && other_line._valid)
        {
            // incrememnt counters for all other lines with count less than old count of hit line
            if (other_line._count < old_count)
            {
                // handle saturation
                if (other_line._count != set_content->size()) {
                    other_line._count = other_line._count + 1;
                }
            }
        }
    }
}

cache_line_states* cache::get_lru_line(std::vector<cache_line_states>& set_content)
{
    unsigned max_count = 0;
    cache_line_states* lru_line_ptr = nullptr;

    for (auto line = set_content.begin(); line != set_content.end(); line++)
    {
        if (line->_count >= max_count && line->_valid) {
            lru_line_ptr = &*line;
            max_count = line->_count;
        }
    }

    return lru_line_ptr;
}

void cache::swap_cache_line(unsigned cache_set, cache_line_states* cache_line_ptr, cache_line_states* victim_line_ptr)
{
    // update tag appropriately in main and victim cache
    cache_line_ptr -> _tag = (cache_line_ptr -> _tag << _set_index_bit_size) | cache_set;
    victim_line_ptr -> _tag = victim_line_ptr -> _tag >> _set_index_bit_size;

    // swap data
    std::swap(*cache_line_ptr, *victim_line_ptr);
}

// utility functions

unsigned cache::get_set_num(unsigned addr)
{
    return (addr >> _block_bit_size) & ( (int) std::pow(2, _set_index_bit_size) - 1);
}

unsigned cache::get_cache_tag(unsigned addr)
{
    return addr >> (_block_bit_size + _set_index_bit_size);
}

unsigned cache::get_victim_tag(unsigned addr)
{
    return addr >> _block_bit_size;
}

void cache::print_cache_content()
{
    std::cout << "===== " << name << " contents =====" << std::endl;
    unsigned set_count = 0;
    for (auto set : _v_cache_states)
    {   
        std::cout << " set " << set_count << ": \t";
        for (auto line : set)
        {
            std::ios_base::fmtflags f(std::cout.flags());
            std::cout << std::hex << line._tag;
            std::cout.flags(f);
            if (line._dirty){
                std::cout << " " << "D";
            }
            std::cout << " " << line._count;
            std::cout << "  \t\t";
        }
        std::cout << std::endl;

        set_count = set_count + 1;
    }

    if (_is_victim_cache_en)
    {
        std::cout << "===== VC contents =====" << std::endl;
        std::cout << " set 0: \t";
        for (auto line : _v_victim_cache)
        {
            std::ios_base::fmtflags f(std::cout.flags());
            std::cout << std::hex << line._tag;
            std::cout.flags(f);
            if (line._dirty){
                std::cout << " " << "D";
            }
            std::cout << " " << line._count;
            std::cout << "  \t\t";
        }
        std::cout << std::endl;
    }

}


// cache destructor

cache::~cache()
{

}
