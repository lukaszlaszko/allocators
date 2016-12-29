// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include <boost/memory/memory_block.hpp>

#include <iterator>


namespace boost { namespace memory {
    
template <typename bucketizer_type>
class bucketizer_helper
{
public:
    bucketizer_helper(bucketizer_type& instance);
    
    template <std::size_t bucket>
    auto& get() noexcept;
    
private:
    bucketizer_type& instance_;
};

template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t step>
class bucketizer
{
public:
    using allocator_type = allocator;
    
    static const std::size_t buckets_count = (max - min) / step;
    static const std::size_t min_size = min;
    static const std::size_t max_size = max;
    static const std::size_t step_size = step;
    
    memory_block allocate(std::size_t size);
    void deallocate(memory_block& block);
    
    bool owns(memory_block& block);
    
private:
    using this_bucketizer_type = bucketizer<allocator, min, max, step>;
    friend bucketizer_helper<this_bucketizer_type>;
    
    allocator buckets_[(max - min) / step];
};
    
} }

#include "bucketizer.ipp"
