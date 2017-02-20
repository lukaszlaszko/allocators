// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include <boost/memory/memory_block.hpp>

#include <new>
#include <cstdlib>


namespace boost { namespace memory {

    
inline mallocator* mallocator::self_allocate()
{
    auto result = malloc(sizeof(mallocator));
    if (result == NULL)
        throw std::bad_alloc();
    
    mallocator* instance = reinterpret_cast<mallocator*>(result);
    new (instance) mallocator;
    return instance;
}    
    
inline memory_block mallocator::allocate(std::size_t size)
{
    auto result = malloc(size);
    if (result == NULL)
        throw std::bad_alloc();
    
    return { result, size };
}

inline void mallocator::deallocate(memory_block& block)
{
    free(block.address);   
    block = null_block;
}

inline bool mallocator::owns(memory_block& block)
{
    return block.address != nullptr;
}
    
} }


