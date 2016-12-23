#pragma once

#include <boost/memory/memory_block.hpp>
#include <boost/utils/property.hpp>


namespace boost { namespace memory {

template <typename primary_allocator, typename secondary_allocator>
class fallback_allocator
{
public:
    memory_block allocate(std::size_t size);
    void deallocate(memory_block& block);
    
    bool owns(memory_block& block);
    
    primary_allocator& get_primary() noexcept;
    secondary_allocator& get_secondary() noexcept;
    
private:
    primary_allocator primary_;
    secondary_allocator secondary_;
};
    
} }

#include "fallback_allocator.ipp"

