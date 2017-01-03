// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include <boost/memory/null_allocator.hpp>
#include <boost/memory/mallocator.hpp>
#include <boost/memory/fallback_allocator.hpp>
#include <boost/memory/free_list.hpp>
#include <boost/memory/stack_allocator.hpp>
#include <boost/memory/segregator.hpp>
#include <boost/memory/bucketizer.hpp>
#include <boost/memory/affix_allocator.hpp>
#include <boost/memory/bitmapped_block.hpp>

#include <memory>
#include <new>
#include <unordered_map>
#include <utility>
#include <stdexcept>


template <typename T>
class std_support_allocator
{
public:
    using size_type = std::size_t;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    
    std_support_allocator() throw() 
    { }
    std_support_allocator(const std_support_allocator &a) throw() 
    { }
    template <class U>                    
    std_support_allocator(const std_support_allocator<U> &a) throw()
    { }
    ~std_support_allocator() throw() 
    { }

    template<typename _Tp1>
    struct rebind
    {
        using other = std_support_allocator<_Tp1>;
    };

    pointer allocate(size_type n, const void *hint=0)
    {
        auto block = mallocator_.allocate(n);
        return reinterpret_cast<pointer>(block.address);
    }

    void deallocate(pointer p, size_type n)
    {
        boost::memory::memory_block block { p, n };
        mallocator_.deallocate(block);
    }
    
private:
    boost::memory::mallocator mallocator_;
};

struct nop
{
    template <typename T>
    void operator() (T const &) const noexcept { }
};

template <typename T>
using nop_unique_ptr = std::unique_ptr<T, nop>;

// DEFINE_ALLOCATOR macro
#ifndef DEFINE_ALLOCATOR
#define DEFINE_ALLOCATOR(allocator_type)\
\
using registered_allocator_type = allocator_type; \
using size_map_key_type = void*; \
using size_map_value_type = std::size_t; \
using size_map_allocator_type = \
        std_support_allocator<std::pair< \
                const size_map_key_type, \
                size_map_value_type>>; \
using size_map_type = std::unordered_map< \
        size_map_key_type, \
        size_map_value_type, \
        std::hash<size_map_key_type>, \
        std::equal_to<size_map_key_type>>; \
\
boost::memory::mallocator default_allocator; \
thread_local nop_unique_ptr<allocator_type> allocator_handle; \
thread_local nop_unique_ptr<size_map_type> size_map_handle; \
thread_local bool allocation_pending { false }; \
\
void* operator new(std::size_t sz) \
{ \
    if (allocator_handle && !allocation_pending) \
    { \
        allocation_pending = true; \
        auto block = allocator_handle->allocate(sz); \
        size_map_handle.operator *()[block.address] = block.size; \
        allocation_pending = false; \
        return block.address; \
    } \
    else \
    { \
        auto block = default_allocator.allocate(sz); \
        return block.address; \
    } \
} \
\
void operator delete(void* ptr) noexcept \
{ \
    if (allocator_handle) \
    { \
        auto search_result = size_map_handle->find(ptr); \
        if (search_result != size_map_handle->end()) \
        { \
            memory_block block { ptr, search_result->second }; \
            allocator_handle->deallocate(block); \
        } \
    } \
} \

#endif

// INIT_ALLOCATOR macro
#define INIT_ALLOCATOR() \
do { \
    size_map_type size_map; \
    registered_allocator_type allocator; \
    allocator_handle.reset(&allocator); \
    size_map_handle.reset(&size_map); \
} while (0)

// TEARDOWN_ALLOCATOR macro
#define TEARDOWN_ALLOCATOR() \
do { \
    size_map_handle.release(); \
    allocator_handle.release(); \
} while (0)


