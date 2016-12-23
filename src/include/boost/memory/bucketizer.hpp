#pragma once

#include <boost/memory/memory_block.hpp>


namespace boost { namespace memory {

template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t step>
class bucketizer
{
public:
    memory_block allocate(std::size_t size);
    void deallocate(memory_block& block);
    
    bool owns(memory_block& block);
    
    template <std::size_t bucket>
    allocator& get() noexcept; 
    
    static const std::size_t buckets_count = (max - min) / step;
    
private:
    allocator buckets_[(max - min) / step];
};
    
} }

#include "bucketizer.ipp"
