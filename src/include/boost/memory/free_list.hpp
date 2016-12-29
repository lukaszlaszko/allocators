#pragma once

#include <boost/memory/memory_block.hpp>

#include <iterator>


namespace boost { namespace memory {

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

template <
        typename allocator,
        std::size_t batch_size,
        std::size_t min,
        std::size_t max,
        std::size_t capacity>
class free_list
{
public:
    memory_block allocate(std::size_t size);
    void deallocate(memory_block& block);

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
