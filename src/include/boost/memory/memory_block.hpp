// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once 

#include <cstddef>
#include <cstdint>


namespace boost { namespace memory {

/**
 * @brief Represents an allocated memory block.
 * 
 * @details
 * An allocated memory block points at the address at which is has been allocated 
 * as well as size of this block. 
 */    
struct memory_block
{
    /**
     * @breif Block's address.
     */
    void* address;
    
    /**
     * @brief Block's size.
     */
    std::size_t size;
    
    /**
     * @brief Implicitly converts the structure to the underlying block address.
     *  
     * @return An address to the allocated block. 
     */
    inline operator std::uint8_t* () const
    {
        return reinterpret_cast<std::uint8_t*>(address);
    }
};
    
} }

