#pragma once


namespace boost { namespace memory {
   
template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t step>
inline memory_block bucketizer<
        allocator,
        min,
        max,
        step>::allocate(std::size_t size)
{
    if (min <= size && size <= max)
    {
        auto bucket_index = (size - min) / step;
        auto& bucket = buckets_[bucket_index];
        
        return bucket.allocate(size);
    }
    else
    {
        return { nullptr, 0ul };
    }
}

template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t step>
inline void bucketizer<
        allocator,
        min,
        max,
        step>::deallocate(memory_block& block)
{
    if (min <= block.size && block.size <= max)
    {
        auto bucket_index = (block.size - min) / step;
        auto& bucket = buckets_[bucket_index];
        
        return bucket.deallocate(block);
    }
}

template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t step>
inline bool bucketizer<
        allocator,
        min,
        max,
        step>::owns(memory_block& block)
{
    if (min <= block.size && block.size <= max)
    {
        auto bucket_index = (block.size - min) / step;
        auto& bucket = buckets_[bucket_index];
        
        return bucket.owns(block);
    }
}

template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t step>
template <std::size_t bucket>
inline allocator& bucketizer<
        allocator,
        min,
        max,
        step>::get() noexcept
{
    static_assert(bucket < (max - min) / step, "Wrong bucket number!");
    
    return buckets_[bucket];
}
    
} }
