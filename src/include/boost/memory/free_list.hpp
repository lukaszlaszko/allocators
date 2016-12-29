#pragma once

#include <boost/memory/memory_block.hpp>

#include <iterator>


namespace boost { namespace memory {

/**
 * @brief A helper for accessing some of the private properties of *free_list*
 * @details
 * The helper provides facilities such as accessor for the underlying allocator, iterator
 * over the list elements which can be used for testing free_list. 
 * 
 * @tparam free_list_type A type of free_list which will be extended with this helper.
 */
template <typename free_list_type>
class free_list_helper
{
public:
    class iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = memory_block;
        using difference_type = std::size_t;
        using pointer = memory_block*;
        using reference = memory_block&;
        
        iterator(typename free_list_type::node* current);
        
        bool operator==(const iterator& other);
        bool operator!=(const iterator& other);
        iterator& operator++();
        iterator  operator++(int step) const;
        value_type operator*() const;

    private:
        typename free_list_type::node* current_;
    };

    static constexpr auto node_size = sizeof(typename free_list_type::node);

    free_list_helper(free_list_type& instance);

    auto& get_allocator() noexcept;
    iterator begin();
    iterator end();

private:
    free_list_type& instance_;
};

/**
 * @brief Provides mechanism for reusing deallocated memory blocks.
 * @details
 * Intention of this allocator is to recycle memory blocks allocated with the backing 
 * *allocator* and possibly use them again when requested. The list starts empty. Then on the 
 * first request a batch of *memory_block*s are allocated and placed in the list.
 * The first block is returned immediately as an allocated block the subsequently allocated blocks 
 * are returned further on on subsequent *allocate* calls. Once a block is returned to the list 
 * through *deallocate* call, it's placed at the head of the list, so it'll be used on the very next allocate request. 
 * This keeps the caches warm. The list defines its capacity over which blocks can't be allocated by the underlying 
 * allocator. This mechanism prevents the list from indefinite growth as allocated blocks can't be destroyed.   
 *  
 * @tparam allocator Underlying allocator used to actually allocate the blocks hold by the list.
 * @tparam batch_size Number of allocation which should be requested from the underlying allocator in event the list is empty. 
 * @tparam min Minimal size of a block requested from the list.
 * @tparam max Maximal size of a block requested from the list. 
 * @tparam capacity Capacity of the list. Defines to upper watermark for number of allocations which can be 
 * performed with the underlying allocator.
 */
template <
        typename allocator,
        std::size_t batch_size,
        std::size_t min,
        std::size_t max,
        std::size_t capacity>
class free_list
{
public:
    /**
     * @brief Allocates a *memory_block* from either the list or using the underlying allocator. 
     * @param size Requested block size.
     * @return Either allocated *memory_block* or *{ nullptr, 0ul }* is block can't be allocated. 
     */
    memory_block allocate(std::size_t size);
    
    /**
     * @brief Deallocates the given *memory_block*.
     * @details
     * A block can be deallocated only if it's been allocated by the underlying 
     * allocator. Deallocated block isn't physically deallocated by the underlying allocator,
     * but instead is added to the head of the list and recycled on the next allocation.
     * @param block The *memory_block* to be recycled.
     */
    void deallocate(memory_block& block);

    /**
     * @brief Determines if the list owns the block.
     * @param block The block to check.
     * @return true if the block has been allocated from this list, false otherwise.
     */
    bool owns(memory_block& block);

private:
    using this_free_list_type = free_list<allocator, batch_size, min, max, capacity>;
    friend free_list_helper<this_free_list_type>;

    struct node
    {
        memory_block block_;
        node* next_;
    };

    allocator allocator_;
    node* head_ {nullptr};

    std::size_t allocations_count_ {0ul};
};

} }

#include "free_list.ipp"
