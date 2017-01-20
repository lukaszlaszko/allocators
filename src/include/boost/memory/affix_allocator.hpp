// Copyright (c) 2016 Lukasz Laszko
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#pragma once

#include <boost/memory/memory_block.hpp>
#include <boost/memory/null_allocator.hpp>

#include <type_traits>


namespace boost { namespace memory {

//C++03, §14.7.3/2
// an explicit specialization can be only declared at the namespace level,
// never at a class member scope.
template <typename type>
constexpr std::size_t size_of();

template <>
constexpr std::size_t size_of<void>();

template <typename type>
void construct_at(memory_block& block, std::size_t offset);

template <>
void construct_at<void>(memory_block& block, std::size_t offset);

template <typename type>
void destruct_at(memory_block& block, std::size_t offset);

template <>
void destruct_at<void>(memory_block& block, std::size_t offset);

/**
 * @brief Helper type exposing some of **affix_allocator** internals.
 * 
 * @details
 * This type is not intended to be used from the user code. Instead it allows to 
 * access private member of **affix_allocator** in unit tests. 
 */
template <typename affix_allocator_type>
class affix_allocator_helper final
{
public:
    affix_allocator_helper(affix_allocator_type& instance);

    auto& get_allocator();

private:
    affix_allocator_type& instance_;
};

/**
 * @brief Allocator adding prefix and suffix objects in boundries of  allocated block.
 * 
 * @details
 * 
 * @tparam allocator The underlying allocator performing physical **memory_block**
 * allocation for the requested block, prefix and suffix.
 * @tparam prefix Prefix type, it has to be a default constructable type. An instance of 
 * this type will be constructed just before requested block on **allocate** 
 * and destroyed on **deallocate**. 
 * @tparam suffix  Suffix type, it has to be a default constructable type. An instance of
 * this type will be constructed just before requested block on **allocate** 
 * and destroyed on **deallocate**.
 * @tparam verify A boolean parameter indicating if prefix/suffix destructors should be
 * invoked on **deallocate**.  
 */
template <
        typename allocator,
        typename prefix,
        typename suffix = void,
        bool verify = false>
class affix_allocator
{
public:
    static_assert(
            std::is_default_constructible<prefix>::value || std::is_same<prefix, void>::value, 
            "prefix has to be default constructible or void");
    static_assert(
            std::is_default_constructible<suffix>::value || std::is_same<suffix, void>::value, 
            "suffix has to be default constructible or void");
    
    using allocator_type = allocator;
    using prefix_type = prefix;
    using suffix_type = suffix;

    /**
     * @brief Allocates a **memory_block** with configured affixes.
     * 
     * @details
     * Allocates a **memory block** of the given size using underlying **allocator**. 
     * Additionally **prefix** and **suffix** objects are constructed before and after returned
     * memory block. These can be used for any purpose - tracking, debugging buffer 
     * under and overrun issues.    
     * 
     * @return An instance of **memory_block** for the allocated block or null block 
     * for situations when allocation is not possible.
     */
    memory_block allocate(std::size_t size);
    
    /**
     * @brief Deallocates the given **memory_block**
     * 
     * @details
     * Attempts to deallocate the given memory block by destroying **prefix** and 
     * **suffix** instances affiliated to this block, but only if **verify**==*true*.
     * **memory_block** is reclaimed only if it is owned by the underlying allocator.
     * 
     * @param block The **memory_block** to deallocate.
     */
    void deallocate(memory_block& block);

    /**
     * @brief Determines if the given **memory_block** is owned by this allocator.
     * 
     * @details 
     * Ownership is determined by checking ownership of the underlying allocator 
     * for the complete block with defined **prefix** and **suffix**.
     * 
     * @param block
     * @return *true* if the block is owned by this allocator, *false* otherwise.
     */
    bool owns(memory_block& block);

private:
    using this_affix_allocator_type = affix_allocator<allocator, prefix, suffix, verify>;
    friend affix_allocator_helper<this_affix_allocator_type>;

    static std::size_t size_of_allocation(std::size_t size) noexcept;
    static memory_block offset_block(memory_block& block, std::size_t size) noexcept;
    static memory_block original_block(memory_block& block) noexcept;
    static void construct_prefix(memory_block& block);
    static void construct_suffix(memory_block& block);
    static void verify_prefix(memory_block& block);
    static void verify_suffix(memory_block& block);

    allocator allocator_;
};

} }

#include "affix_allocator.ipp"

