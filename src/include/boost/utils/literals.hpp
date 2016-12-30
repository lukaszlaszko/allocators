#pragma once


inline std::uint8_t operator "" _ui8(unsigned long long value)
{
    return static_cast<std::uint8_t>(value);
}

