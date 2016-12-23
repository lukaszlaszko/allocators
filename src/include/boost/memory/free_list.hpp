#pragma once

#include <boost/memory/memory_block.hpp>


namespace boost { namespace memory {

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
    struct node
    {
        memory_block block_;
        node* next_;
    };
    
    allocator allocator_;
    node* head_;
    
    std::size_t allocations_count_;
};
    
} }

#include "free_list.ipp"
