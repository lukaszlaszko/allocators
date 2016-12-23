#pragma once

#include <boost/memory/memory_block.hpp>


namespace boost { namespace memory {

/**
 * @brief Segregates allocation strategies according to the given allocation size 
 * threshold. 
 * 
 * @details
 * Segregates the given allocators in a way it uses the first one for allocation
 * of all blocks which size is less or equal to the given threshold and the second
 * allocator for blocks larger than the threshold.   
 * 
 * @tparam threshold Size threshold used as segregation criteria.
 * @tparam primary_allocator Allocator used for allocation of blocks smaller or 
 * equal to threshold.
 * @tparam secondary_allocator Allocator used for allocation of blocks larger 
 * than threshold.
 */
template <
        std::size_t threshold, 
        typename primary_allocator, 
        typename secondary_allocator>
class segregator
{
public:
    /**
     * @brief Allocates a memory block of the given size.
     * 
     * @param size Size of a block to be allocated.
     * @return An allocated memory block.
     */
    memory_block allocate(std::size_t size);
    
    /**
     * @brief Deallocates the given *memory_block* if possible.
     * 
     * @param block The block to deallocate.
     */
    void deallocate(memory_block& block);
    
    /**
     * @brief Checks if the given memory block has been allocated by this allocator.
     * 
     * @param block A block to check.
     * @return true if the block has been allocated by this allocator, false otherwise.
     */
    bool owns(memory_block& block);
    
    /**
     * @brief Gets a reference to the primary allocator. 
     * 
     * @return A reference to the primary allocator.
     */
    primary_allocator& get_primary() noexcept;
    
    /**
     * @brief Gets a reference to the secondary allocator.
     * 
     * @return A reference to the secondary allocator.
     */
    secondary_allocator& get_secondary() noexcept;
    
private:
    primary_allocator primary_;
    secondary_allocator secondary_;
};
    
} }

#include "segregator.ipp"

