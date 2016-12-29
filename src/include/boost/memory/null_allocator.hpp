// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include <boost/memory/memory_block.hpp>

#include <cstddef>


namespace boost { namespace memory {
  
/**
 * @brief An allocator acting as the last fallback allocator, allocating only nullptr.
 * 
 * @details
 * It's a terminal allocator capable of allocating only { nullptr, 0ul } blocks.
 */
class null_allocator
{
public:
    /**
     * @breif Always allocates { nullptr, 0ul } block.
     * 
     * @param size Requested allocation size.
     * @return Allocated memory block description. 
     */
    memory_block allocate(std::size_t size = 0);
    
    /**
     * @breif Does nothing.
     *  
     * @param block A block to deallocate.
     */
    void deallocate(memory_block& block);
    
    /**
     * @brief Checks if the given *block* is owned by this allocator.
     * 
     * @param block A *memory_block* which ownership is going to be checked.
     * @return true if this allocator owns the given block, false otherwise. 
     */
    bool owns(memory_block& block);
};
    
} }

#include "null_allocator.ipp"

