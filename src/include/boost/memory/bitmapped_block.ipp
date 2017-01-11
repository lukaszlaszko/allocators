// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include <boost/memory/alignment.hpp>
#include <boost/utils/literals.hpp>

#include <cstring>


namespace boost { namespace memory {
    
template <typename bitmapped_block_type>
inline bitmapped_block_helper<bitmapped_block_type>::bitmapped_block_helper(bitmapped_block_type& instance) noexcept
        :
    instance_(instance)
{
    
}

template <typename bitmapped_block_type>
inline auto& bitmapped_block_helper<bitmapped_block_type>::get_allocator() noexcept
{
    return instance_.allocator_;
}

template <typename bitmapped_block_type>
inline auto& bitmapped_block_helper<bitmapped_block_type>::get_bitmap() noexcept
{
    return *instance_.bitmap_;
}

//template <typename bitmapped_block_type>
//inline auto& bitmapped_block_helper::get_block() noexcept
//{
//    return 
//}

template <typename bitmap_type>
inline bitmap_helper<bitmap_type>::bitmap_helper(bitmap_type& instance) noexcept
        :
    instance_(instance)
{
    
}

template <typename bitmap_type>
inline std::uint8_t& bitmap_helper<bitmap_type>::get_flag(std::size_t index)
{
    return instance_.flags_[index];
}

template <typename bitmap_type>
inline constexpr std::size_t bitmap_helper<bitmap_type>::flags_count() const noexcept
{
    return sizeof(instance_.flags_);
}

template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t capacity, 
        std::size_t alignment>
template <
        std::size_t size>
inline void bitmapped_block<
        allocator,
        min,
        max,
        capacity,
        alignment>::bitmap<size>::set(std::size_t index)
{
    auto& flag = flags_[index / 8ul];
    flag |= flag_mask(index % 8ul);    
}

template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t capacity, 
        std::size_t alignment>
template <
        std::size_t size>
inline void bitmapped_block<
        allocator,
        min,
        max,
        capacity,
        alignment>::bitmap<size>::reset(std::size_t index)
{
    auto& flag = flags_[index / 8ul];
    flag &= ~flag_mask(index % 8ul);
}

template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t capacity, 
        std::size_t alignment>
template <
        std::size_t size>
inline void bitmapped_block<
        allocator,
        min,
        max,
        capacity,
        alignment>::bitmap<size>::reset_all()
{
    std::memset(flags_, 0, size);
}

template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t capacity, 
        std::size_t alignment>
template <
        std::size_t size>
inline bool bitmapped_block<
        allocator,
        min,
        max,
        capacity,
        alignment>::bitmap<size>::is_set(std::size_t index)
{
    auto& flag = flags_[index / 8ul];
    return (flag & flag_mask(index % 8ul)) != 0_ui8;
}

template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t capacity, 
        std::size_t alignment>
template <
        std::size_t size>
inline bool bitmapped_block<
        allocator,
        min,
        max,
        capacity,
        alignment>::bitmap<size>::claim(std::size_t& index)
{
    for (auto i = 0ul; i < size; i++)
    {
        auto& flag = flags_[i];
        if ((flag & all_mask) != all_mask)
        {
            for (auto j = 0_ui8; j < 8_ui8; j++)
            {
                auto mask = flag_mask(j);
                if ((flag & mask) == 0_ui8)
                {
                    flag |= mask;
                    
                    index = i * 8ul + j;
                    return true;
                }
            }
        }
    }
    
    return false;
}
    
template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t capacity, 
        std::size_t alignment>
inline memory_block bitmapped_block<
        allocator,
        min,
        max,
        capacity,
        alignment>::allocate(std::size_t size)
{
    if (size < min || max < size)
        return null_allocator_.allocate();
    
    if (allocated_block_ == nullptr)
    {
        auto allocation_size = sizeof(bitmap_type) 
                + aligned_max_size * capacity 
                + (alignment > NO_ALIGNMENT ? alignment : 0ul);
        allocated_block_ = allocator_.allocate(allocation_size);
        
        bitmap_ = reinterpret_cast<bitmap_type*>(allocated_block_.address);
        bitmap_->reset_all();
    }
    
    std::size_t index;
    if (bitmap_->claim(index) && index < capacity)
    {
        auto first_address = align_forward<alignment>(allocated_block_ + sizeof(bitmap_type));
        auto claimed_address = first_address + aligned_max_size * index;
        
        return { claimed_address, aligned_max_size };
    }
    else
    {
        return null_allocator_.allocate();
    }
}

template <
        typename allocator, 
        std::size_t min,
        std::size_t max,
        std::size_t capacity, 
        std::size_t alignment>
inline void bitmapped_block<
        allocator,
        min,
        max,
        capacity,
        alignment>::deallocate(memory_block& block)
{
    if (owns(block))
    {
        auto first_address = align_forward<alignment>(allocated_block_ + sizeof(bitmap_type));
        auto offset = block - first_address;
        auto index = offset / aligned_max_size;
        
        bitmap_->reset(index);
        null_allocator_.deallocate(block);
    }
}

template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t capacity, 
        std::size_t alignment>
inline bool bitmapped_block<
        allocator,
        min,
        max,
        capacity,
        alignment>::owns(memory_block& block)
{
    if (allocated_block_ == nullptr)
        return false;
    
    auto start_address = align_forward<alignment>(allocated_block_ + sizeof(bitmap_type));
    if (block < start_address)
        return false;
    
    auto allocation_size = sizeof(bitmap_type) 
                + aligned_max_size * capacity 
                + alignment;
    auto end_address = allocated_block_ + allocation_size;
    if (end_address < block)
        return false;
    
    auto offset = block - start_address;
    return offset % aligned_max_size == 0ul;
}
    
} }

