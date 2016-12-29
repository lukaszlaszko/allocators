#pragma once


namespace boost { namespace memory {    
  
inline memory_block null_allocator::allocate(std::size_t size)
{
    return { nullptr, 0ul };
}

inline void null_allocator::deallocate(memory_block& block)
{
    block.address = nullptr;
    block.size = 0ul;
}

inline bool null_allocator::owns(memory_block& block)
{
    return block.address == nullptr;
}
    
} }

