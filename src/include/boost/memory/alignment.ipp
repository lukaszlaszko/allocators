// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include "alignment.hpp"


namespace boost { namespace memory {
    
static constexpr bool is_power_of_2(std::size_t x) noexcept
{
    while (((x & 1) == 0) && x > 1)
        x >>= 1;
    return (x == 1);
}    
    
template <>
inline std::uint8_t* align_forward<1>(std::uint8_t* address) noexcept
{
    return address;
}    

template <std::size_t alignment>
inline std::uint8_t* align_forward(std::uint8_t* address) noexcept
{
    static_assert(is_power_of_2(alignment), "alignment must be a power of 2");
    
    auto address_ptr = reinterpret_cast<std::uintptr_t>(address);
    return address_ptr % alignment == 0
         ? address
         : reinterpret_cast<std::uint8_t*>(address_ptr + (alignment - (address_ptr % alignment)));
}

template <>
inline std::uint8_t* align_backward<1>(std::uint8_t* address) noexcept
{
    return address;
}    

template <std::size_t alignment>
inline std::uint8_t* align_backward(std::uint8_t* address) noexcept
{
    static_assert(is_power_of_2(alignment), "alignment must be a power of 2");
    
    auto address_ptr = reinterpret_cast<std::uintptr_t>(address);
    return address_ptr % alignment == 0
         ? address
         : reinterpret_cast<std::uint8_t*>(address_ptr - (address_ptr % alignment));
}
    
} }

