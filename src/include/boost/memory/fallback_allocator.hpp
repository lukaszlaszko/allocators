// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include <boost/memory/memory_block.hpp>


namespace boost { namespace memory {

/**
 * @brief Allocates with secondary allocator, if allocation with primary allocator 
 * fails. 
 * 
 * @details
 * This type of allocator allows construction of allocator chains, where allocation
 * is automatically passed over to the secondary allocator in event the primary allocation
 * cannot succeed due to conditions imposed on the allocator.
 * 
 * @tparam primary_allocator The primary allocator type to be used.
 * @tparam secondary_allocator The secondary allocator type to be used.
 */    
template <
        typename primary_allocator, 
        typename secondary_allocator>
class fallback_allocator final
{
public:
    /**
     * @brief Allocates a **memory_block** with either *primary_allocator* or *secondary_allocator*.
     * 
     * @details
     * @par
     * An allocation through **fallback_allocator** at first attempts to allocate 
     * requested block with **primary_allocator**. Should it fail, allocation with 
     * **secondary_allocator** is exercised.
     *    
     * @par
     * If both backing allocator fail to allocate desired block, a **null_block** 
     * is returned.
     * 
     * @param size Desired size of the newly allocated block.
     * @return **memory_block** non-equal to **null_block** if allocation was successful,
     * or **null_block** otherwise. 
     */
    memory_block allocate(std::size_t size);
    
    /**
     * @brief Deallocates the given **memory_block**.
     * 
     * @details
     * The given **memory_block** is only deallocated if it's owned by either of 
     * the backing allocators and should be handled for deallocation to an instance 
     * of that allocator.
     * 
     * @param block The **memory_block** to deallocate.
     */
    void deallocate(memory_block& block);
    
    /**
     * @brief Examines if the given block is owned by this allocator.
     * 
     * @details
     * A **memory_block** is owned by a fallback allocator only if it's owned by 
     * either of the backing allocators. 
     * 
     * @param block A **memory_block** to examine.
     * @return *true* if the **block** is owned by the allocator.
     */
    bool owns(memory_block& block);
    
    primary_allocator& get_primary() noexcept;
    secondary_allocator& get_secondary() noexcept;
    
private:
    primary_allocator primary_;
    secondary_allocator secondary_;
};
    
} }

#include "fallback_allocator.ipp"

