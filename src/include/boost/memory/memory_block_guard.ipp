#pragma once

#include "memory_block_guard.hpp"


namespace boost { namespace memory {
    
template <typename allocator_type>
memory_block_guard<allocator_type>::memory_block_guard(allocator_type& allocator, size_t size)
    :
        allocator_(allocator),
        allocated_block_(allocator.allocate(size))
{
    
}

template <typename allocator_type>
memory_block_guard<allocator_type>::~memory_block_guard()
{
    if (allocated_block_ != null_block)
        allocator_.deallocate(allocated_block_);
}

template <typename allocator_type>
memory_block_guard<allocator_type>::operator memory_block() const
{
    return allocated_block_;
}
    
} }

