// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include "null_allocator.hpp"



namespace boost { namespace memory {
  
template <
        std::size_t capacity, 
        std::size_t alignment>
inline memory_block stack_allocator<capacity, alignment>::allocate(std::size_t size)
{
    auto end = reserved_block_ + capacity;
    auto next = align_forward<alignment>(current_ + size);
    if (next <= end)
    {
        memory_block block { current_, size };
        current_ = next;
        
        return block;
    }
    else
    {
        return null_allocator_.allocate();
    }
}

template <
        std::size_t capacity,
        std::size_t alignment>
inline void stack_allocator<capacity, alignment>::deallocate(memory_block& block)
{
    auto previous = align_backward<alignment>(current_ - block.size);
    if (previous == block.address)
    {
        current_ = previous;
        block = { nullptr, 0ul };
    }
}

template <
        std::size_t capacity,
        std::size_t alignment>
inline bool stack_allocator<capacity, alignment>::owns(memory_block& block) const noexcept
{
    return &reserved_block_ <= block.address && block.address <= &reserved_block_ + capacity;
}
    
} }

