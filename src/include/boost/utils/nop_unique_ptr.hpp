#pragma once

#include <memory>


namespace boost { namespace utils {
  
struct nop
{
    template <typename T>
    void operator() (T const &) const noexcept { }
};

template <typename T>
using nop_unique_ptr = std::unique_ptr<T, nop>;
    
} }

