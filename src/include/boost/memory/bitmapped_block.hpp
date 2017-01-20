// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include <boost/memory/alignment.hpp>
#include <boost/memory/null_allocator.hpp>
#include <boost/memory/memory_block.hpp>


namespace boost { namespace memory {

    
/**
 * @brief
 * @details
 * This helper is not intended to be used in the user code.
 */
template <typename bitmapped_block_type>
class bitmapped_block_helper final
{
public:
    bitmapped_block_helper(bitmapped_block_type& instance) noexcept;

    auto& get_allocator() noexcept;
    auto& get_bitmap() noexcept;
    //auto& get_block(std::size_t index);

private:
    bitmapped_block_type& instance_;
};

template <typename bitmap_type>
class bitmap_helper
{
public:
    bitmap_helper(bitmap_type& instance) noexcept;
    
    std::uint8_t& get_flag(std::size_t index);
    constexpr std::size_t flags_count() const noexcept;
    
private:
    bitmap_type& instance_;
};

/**
 * @brief A continous memory block divided into equal number of sub-block with a bitmap
 * indicating availability of such sub-blocks.
 * 
 * @details
 * 
 * @tparam allocator
 * @tparam min
 * @tparam max
 * @tparam capacity
 * @tparam alignment 
 */
template <
        typename allocator,
        std::size_t min,
        std::size_t max,
        std::size_t capacity,
        std::size_t alignment=NO_ALIGNMENT>
class bitmapped_block
{
public:
    static_assert(is_power_of_2(capacity), "capacity has to be a power of 2!");
    static_assert(alignment > 0, "alignment has to be equal or greater than 1!");

    /**
     * @brief A map of bit flags  
     */
    template <std::size_t size>
    class bitmap
    {
    public:
        void set(std::size_t index);
        void reset(std::size_t index);
        void reset_all();
        bool is_set(std::size_t index);
        
        bool claim(std::size_t& index);

    private:
        using bitmap_type = bitmap<size>;
        friend bitmap_helper<bitmap_type>;
        
        static constexpr std::uint8_t all_mask = 0b11111111;
        static constexpr std::uint8_t flag_mask(std::uint8_t index)
        {
            std::uint8_t flag_masks[8] = {
                0b10000000,
                0b01000000,
                0b00100000,
                0b00010000,
                0b00001000,
                0b00000100,
                0b00000010,
                0b00000001
            };
            
            return flag_masks[index];
        };
        
        std::uint8_t flags_[size] {};
    };

    using allocator_type = allocator;
    using bitmap_type = bitmap<(capacity % 8 == 0 ? capacity / 8 : capacity / 8 + 1)>;
    
    static const std::size_t min_size = min;
    static const std::size_t max_size = max;
    static const std::size_t aligned_max_size = align_size<alignment>(max);
    static const std::size_t blocks_count = capacity;

    memory_block allocate(std::size_t size);
    void deallocate(memory_block& block);

    bool owns(memory_block& block);

private:
    using this_bitmapped_block = bitmapped_block<allocator, min, max, capacity, alignment>;
    friend bitmapped_block_helper<this_bitmapped_block>;

    allocator allocator_;
    
    memory_block allocated_block_ { nullptr, 0ul };
    bitmap_type* bitmap_ { nullptr };
};

} }

#include "bitmapped_block.ipp"
