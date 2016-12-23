#pragma once


namespace boost { namespace memory {
  
template <
        std::size_t threshold, 
        typename primary_allocator, 
        typename secondary_allocator>
inline memory_block segregator<
        threshold, 
        primary_allocator, 
        secondary_allocator>::allocate(std::size_t size)
{
    if (size <= threshold)
        return primary_.allocate(size);
    else
        return secondary_.allocate(size);
}

template <
        std::size_t threshold, 
        typename primary_allocator, 
        typename secondary_allocator>
inline void segregator<
        threshold, 
        primary_allocator, 
        secondary_allocator>::deallocate(memory_block& block)
{
    if (block.size <= threshold)
        primary_.deallocate(block);
    else
        secondary_.deallocate(block);
}

template <
        std::size_t threshold, 
        typename primary_allocator, 
        typename secondary_allocator>
inline bool segregator<
        threshold, 
        primary_allocator, 
        secondary_allocator>::owns(memory_block& block)
{
    return block.size <= threshold ? primary_.owns(block) : secondary_.owns(block);
}

template <
        std::size_t threshold, 
        typename primary_allocator, 
        typename secondary_allocator>
inline primary_allocator& segregator<
        threshold, 
        primary_allocator, 
        secondary_allocator>::get_primary() noexcept
{
    return primary_;
}

template <
        std::size_t threshold, 
        typename primary_allocator, 
        typename secondary_allocator>
inline secondary_allocator& segregator<
        threshold, 
        primary_allocator, 
        secondary_allocator>::get_secondary() noexcept
{
    return secondary_;
}
    
} }