// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include <boost/memory/memory_block.hpp>


namespace boost { namespace memory {

template <typename primary_allocator, typename secondary_allocator>
class fallback_allocator
{
public:
    memory_block allocate(std::size_t size);
    void deallocate(memory_block& block);
    
    bool owns(memory_block& block);
    
    primary_allocator& get_primary() noexcept;
    secondary_allocator& get_secondary() noexcept;
    
private:
    primary_allocator primary_;
    secondary_allocator secondary_;
};
    
} }

#include "fallback_allocator.ipp"

