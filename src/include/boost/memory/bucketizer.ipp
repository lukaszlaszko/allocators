#pragma once


namespace boost { namespace memory {
  
template <typename bucketizer_type>
bucketizer_helper<bucketizer_type>::bucketizer_helper(bucketizer_type& instance)
        :
    instance_(instance)
{
    
}

template <typename bucketizer_type>
template <std::size_t bucket>
inline auto& bucketizer_helper<bucketizer_type>::get() noexcept
{
    static const auto min = bucketizer_type::min_size;
    static const auto max = bucketizer_type::max_size;
    static const auto step = bucketizer_type::step_size;
    
    static_assert(bucket < (max - min) / step, "Wrong bucket number!");
    
    return instance_.buckets_[bucket];
}    
    
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
    else
    {
        return false;
    }
}
    
} }
