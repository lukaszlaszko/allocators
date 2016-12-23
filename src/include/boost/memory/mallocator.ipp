#pragma once

#include "memory_block.hpp"

#include <cstdlib>
#include <stdexcept>


namespace boost { namespace memory {
  
inline memory_block mallocator::allocate(std::size_t size)
{
    auto result = malloc(size);
    if (result == NULL)
        throw std::runtime_error("malloc failed!");
    
    return { result, size };
}

inline void mallocator::deallocate(memory_block& block)
{
    free(block.address);
    
    block.address = nullptr;
    block.size = 0ul;
}

inline bool mallocator::owns(memory_block& block)
{
    return block.address != nullptr;
}
    
} }


