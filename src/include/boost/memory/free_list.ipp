#pragma once

#include "memory_block.hpp"
#include "free_list.hpp"

#include <cassert>


namespace boost { namespace memory {

template <typename free_list_type>
inline free_list_helper<free_list_type>::free_list_helper(free_list_type& instance)
        :
    instance_(instance)
{
}

template <typename free_list_type>
inline auto& free_list_helper<free_list_type>::get_allocator() noexcept
{
    return instance_.allocator_;
}

template <typename free_list_type>
inline typename free_list_helper<free_list_type>::iterator free_list_helper<free_list_type>::begin()
{
    return iterator(instance_.head_);
}

template <typename free_list_type>
inline typename free_list_helper<free_list_type>::iterator free_list_helper<free_list_type>::end()
{
    return iterator(nullptr);
}

template <typename free_list_type>
inline free_list_helper<free_list_type>::iterator::iterator(typename free_list_type::node* current)
        :
    current_(current)
{
}

template <typename free_list_type>
inline bool free_list_helper<free_list_type>::iterator::operator ==(const free_list_helper<free_list_type>::iterator& other)
{
    return current_ == other.current_;
}

template <typename free_list_type>
inline bool free_list_helper<free_list_type>::iterator::operator !=(const free_list_helper<free_list_type>::iterator& other)
{
    return current_ != other.current_;
}

template <typename free_list_type>
inline typename free_list_helper<free_list_type>::iterator& free_list_helper<free_list_type>::iterator::operator ++()
{
    current_ = current_ != nullptr ? current_->next_ : nullptr;
    return *this;
}

template <typename free_list_type>
inline typename free_list_helper<free_list_type>::iterator free_list_helper<free_list_type>::iterator::operator ++(int step) const
{
    iterator advanced(current_);
    for (auto i = 0; i < step && advanced.current_ != nullptr; i++)
        advanced++;

    return advanced;
}

template <typename free_list_type>
inline memory_block free_list_helper<free_list_type>::iterator::operator *() const
{
    return current_->block_;
}

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
            memory_block first { nullptr, 0ul };

            for (auto i = 0ul; i < batch_size; i++)
            {
                if (allocations_count_++ == capacity)
                    break;

                auto node_with_block = allocator_.allocate(sizeof(node) + max);
                auto current_node = reinterpret_cast<node*>(node_with_block.address);
                current_node->block_ = { node_with_block + sizeof(node), max };

                if (i == 0ul)
                {
                    first = current_node->block_;
                }
                else
                {
                    current_node->next_ = head_;
                    head_ = current_node;
                }
            }

            return first;
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
        
        block = { nullptr, 0ul };
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

