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
     * @brief Allocates a *memory_block* using *malloc*.
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

