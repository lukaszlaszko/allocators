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

#include <boost/utils/backtrace.hpp>

#include <memory>
#include <new>
#include <unordered_map>
#include <utility>
#include <stdexcept>


struct nop
{
    template <typename T>
    void operator() (T const &) const noexcept { }
};

template <typename T>
using nop_unique_ptr = std::unique_ptr<T, nop>;

// DEFINE_ALLOCATOR macro
#ifndef DEFINE_ALLOCATOR
#define DEFINE_ALLOCATOR_FULL(allocator_type, backtrace_enabled) \
\
using registered_allocator_type = allocator_type; \
using size_map_key_type = void*; \
using size_map_value_type = std::size_t; \
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
        if (backtrace_enabled) \
        { \
            boost::utils::backtrace bt; \
            std::cerr << "++++ custom allocator at " << std::endl; \
            std::cerr << bt << std::endl; \
        } \
        allocation_pending = true; \
        auto block = allocator_handle->allocate(sz); \
        size_map_handle.operator *()[block.address] = block.size; \
        allocation_pending = false; \
        return block.address; \
    } \
    else \
    { \
        if (backtrace_enabled) \
        { \
            boost::utils::backtrace bt; \
            std::cerr << "++++ default allocator at " << std::endl; \
            std::cerr << bt << std::endl; \
        } \
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
    else \
    { \
        memory_block block { ptr, 1ul }; \
        default_allocator.deallocate(block); \
    } \
}

#define DEFINE_ALLOCATOR(allocator_type) \
    DEFINE_ALLOCATOR_FULL(allocator_type, false)

#define DEFINE_ALLOCATOR_WITH_TRACE(allocator_type) \
    DEFINE_ALLOCATOR_FULL(allocator_type, true)

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


