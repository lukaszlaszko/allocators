#pragma once

#include <cstddef>
#include <cstdint>


namespace boost { namespace memory {

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
inline std::uint8_t* align_forward(std::uint8_t* address) noexcept;

/**
 * @brief Aligns the given address backwards to the given alignment boundary.
 * 
 * Aligns the address to itself or any lower address dividable by *alignment*.
 * 
 * @param address An address to be aligned.
 * @return Address aligned forward to 
 */
template <std::size_t alignment>
inline std::uint8_t* align_backward(std::uint8_t* address) noexcept;

} }

#include "alignment.ipp"

