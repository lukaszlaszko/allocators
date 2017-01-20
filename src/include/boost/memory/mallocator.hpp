// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include "memory_block.hpp"

namespace boost { namespace memory {
 
/**
 * @brief An allocator backed by *malloc* and *free* system calls.
 */
class mallocator
{
public:
    /**
     * @brief Allocates an instance of mallocator on the heap.
     * @details
     * 
     * In certain situations it's required to allocate an instance of this allocator
     * on the heap. This method is used to do so.
     * @return A pointer to self allocated instance of mallocator.
     */
    static mallocator* self_allocate();
    
    /**
     * @brief Allocates a *memory_block* using *malloc*.
     * 
     * @details
     * 
     * 
     * @param size
     * @return 
     */
    memory_block allocate(std::size_t size);
    
    /**
     * @brief Deallocates memory block using *free*.
     * 
     * @param block A *memory_block* to deallocate.
     */
    void deallocate(memory_block& block);
    
    /**
     * @brief Checks if the block has been allocated with this allocator. 
     * @param block The block to check.
     * @return true if allocated with this allocator, false otherwise.
     */
    bool owns(memory_block& block);
};
    
} }

#include "mallocator.ipp"

