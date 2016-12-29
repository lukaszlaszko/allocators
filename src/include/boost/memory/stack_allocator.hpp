#pragma once

#include <boost/memory/alignment.hpp>
#include <boost/memory/memory_block.hpp>
#include <boost/memory/null_allocator.hpp>

#include <cstdint>


namespace boost { namespace memory {

/**
 * @brief Allocates memory block from a fixed range reserved from the current 
 * execution stack.
 * 
 * @tparam capacity Number of bytes to reserve on the stack.
 * @tparam alignment Defines alignment boundary for allocated blocks. 
 */
template <std::size_t capacity, std::size_t alignment=1>
class stack_allocator
{
public:
    /**
     * @brief Allocated a block of the requested *size* on the stack.
     * @details
     * Retrieves location and size of block allocated within execution stack range
     * reserved by this allocator. The location is aligned according to value of 
     * *alignment* template parameter.
     * @param size The size of block to be allocated.
     * @return A valid *memory_block* definition or { nullptr, 0ul } if memory block
     * can't be allocated. 
     */
    memory_block allocate(std::size_t size);
    
    /**
     * @brief Deallocates the block if possible.
     * @param block A *memory_block* to deallocate.
     */
    void deallocate(memory_block& block);

    /**
     * @brief Determines if the *block* has been allocated with this allocator. 
     * @param block A *memory_block* to examine.
     * @return true if allocated by this allocator, false otherwise. 
     */
    bool owns(memory_block& block) const noexcept;

private:
    std::uint8_t reserved_block_[capacity];
    std::uint8_t* current_ { align_forward<alignment>(reserved_block_) };
    
    null_allocator null_allocator_;
};

} }

#include "stack_allocator.ipp"

