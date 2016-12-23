#pragma once

#include "memory_block.hpp"


namespace boost { namespace memory {
  
template <typename primary_allocator, typename secondary_allocator>
inline memory_block fallback_allocator<primary_allocator, secondary_allocator>::allocate(std::size_t size)
{
    auto block = primary_.allocate(size);
    if (block.address != nullptr)
        return block;
    else
        return secondary_.allocate(size);
}

template <typename primary_allocator, typename secondary_allocator>
inline void fallback_allocator<primary_allocator, secondary_allocator>::deallocate(memory_block& block)
{
    if (primary_.owns(block))
        primary_.deallocate(block);
    else if (secondary_.owns(block))
        secondary_.deallocate(block);
}

template <typename primary_allocator, typename secondary_allocator>
inline bool fallback_allocator<primary_allocator, secondary_allocator>::owns(memory_block& block)
{
    return primary_.owns(block) || secondary_.owns(block);
}

template <typename primary_allocator, typename secondary_allocator>
primary_allocator& fallback_allocator<primary_allocator, secondary_allocator>::get_primary() noexcept
{
    return primary_;
}

template <typename primary_allocator, typename secondary_allocator>
secondary_allocator& fallback_allocator<primary_allocator, secondary_allocator>::get_secondary() noexcept
{
    return secondary_;
}
    
} }

