// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include <cstddef>
#include <cstdint>


namespace boost { namespace memory {
    
/**
 * No alignment.
 */
static constexpr std::size_t NO_ALIGNMENT = 1ul;

#if defined(__amd64__) || defined(__aarch64__)
/**
 * Alignment to word size. On some platforms it's required to access data on 
 * word or half-word aligned addresses.
 * http://www.cs.umd.edu/class/sum2003/cmsc311/Notes/Data/aligned.html
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/BABFAIGG.html
 */
static constexpr std::size_t WORD_ALIGNMENT = 8ul;
static constexpr std::size_t HALF_WORD_ALIGNMENT = 4ul;
#elif defined(__i686__) || defined(__arm__)
static constexpr std::size_t WORD_ALIGNMENT = 4ul;
static constexpr std::size_t HALF_WORD_ALIGNMENT = 2ul;
#else
#   error Unsupported CPU architecture
#endif

/**
 * @brief Helper function used to identify if the given number is a power of 2. 
 * @param x The given number.
 * @return *true* if the number is power of 2, false otherwise.  
 */
static constexpr bool is_power_of_2(std::size_t x) noexcept;  

/**
 * @brief Aligns the given address forward to the given alignment boundary.
 * @details
 * Aligns the address to itself or any higher address dividable by *alignment*.
 * 
 * @param address An address to be aligned.
 * @return Address aligned forward to 
 */
template <std::size_t alignment>
inline constexpr std::uint8_t* align_forward(std::uint8_t* address) noexcept;

/**
 * @brief Aligns the given address backwards to the given alignment boundary.
 * 
 * Aligns the address to itself or any lower address dividable by *alignment*.
 * 
 * @param address An address to be aligned.
 * @return Address aligned forward to 
 */
template <std::size_t alignment>
inline constexpr std::uint8_t* align_backward(std::uint8_t* address) noexcept;

/**
 * @brief Aligns size to the closes multiplication of the alignment
 * @details
 * 
 * Finds the closes multiplication of alignment for the given size.
 * 
 * @param size The size to align
 * @return Aligned size.
 */
template <std::size_t alignment>
inline constexpr std::size_t align_size(std::size_t size) noexcept;

} }

#include "alignment.ipp"

