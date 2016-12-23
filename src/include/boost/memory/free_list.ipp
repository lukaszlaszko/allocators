#pragma once

#include "memory_block.hpp"
#include "free_list.hpp"


namespace boost { namespace memory {
    
template <
    typename allocator,
        std::size_t batch_size, 
        std::size_t min, 
        std::size_t max, 
        std::size_t capacity>
inline memory_block free_list<
        allocator,
        batch_size,
        min,
        max,
        capacity>::allocate(std::size_t size)
{
    if (min <= size && size <= max)
    {
        if (head_ != nullptr)
        {
            auto block = head_->block_;
            head_ = head_->next_;
            
            return block;
        }
        else
        {
            memory_block last { nullptr, 0ul };
            
            for (auto i = 0ul; i < batch_size; i++)
            {
                if (allocations_count_++ == capacity)
                    return last;
                
                auto node_with_block = allocator(sizeof(node) + max);
                auto current_node = reinterpret_cast<node*>(node_with_block);
                current_node->block_ = { node_with_block + sizeof(node), max };
                
                last = current_node->block_;
                current_node->next_ = head_;
                head_ = current_node;
            }
        }
    }
    else
    {
        return { nullptr, 0ul };
    }
}

template <
    typename allocator,
        std::size_t batch_size, 
        std::size_t min, 
        std::size_t max, 
        std::size_t capacity>
inline void free_list<
        allocator,
        batch_size,
        min,
        max,
        capacity>::deallocate(memory_block& block)
{
    if (owns(block))
    {
        auto current_node = reinterpret_cast<node*>(
                reinterpret_cast<std::uint8_t*>(block.address) - sizeof(node));
        current_node->next_ = head_;
        head_ = current_node;
    }
}

template <
    typename allocator,
        std::size_t batch_size, 
        std::size_t min, 
        std::size_t max, 
        std::size_t capacity>
inline bool free_list<
        allocator,
        batch_size,
        min,
        max,
        capacity>::owns(memory_block& block)
{
    if (min <= block.size && block.size <= max)
    {
        return allocator_.owns(block);
    }
    else
    {
        return false;
    }
}
    
} }

