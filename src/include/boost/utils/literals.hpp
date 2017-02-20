#pragma once

/**
 * @brief A literal for producing uint8_t from string literals.
 * @param value A literal for created uint8_t
 * @return uint8_t value. 
 */
inline std::uint8_t operator "" _ui8(unsigned long long value)
{
    return static_cast<std::uint8_t>(value);
}

