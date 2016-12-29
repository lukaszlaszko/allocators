#pragma once

#include <boost/memory/memory_block.hpp>

#include <cstring>

#include "affix_allocator.hpp"


namespace boost { namespace memory {

template <typename type>
constexpr std::size_t size_of()
{
    return sizeof(type);
}

template <>
constexpr std::size_t size_of<void>()
{
    return 0ul;
}

template <typename type>
void construct_at(memory_block& block, std::size_t offset)
{
    new (block + offset) type();
}

template <>
void construct_at<void>(memory_block& block, std::size_t offset)
{
}

template <typename type>
void destruct_at(memory_block& block, std::size_t offset)
{
    reinterpret_cast<type*>(block + offset)->~type();
}

template <>
void destruct_at<void>(memory_block& block, std::size_t offset)
{
}

template <
        typename affix_allocator_type>
inline affix_allocator_helper<
        affix_allocator_type>::affix_allocator_helper(affix_allocator_type& instance)
        :
    instance_(instance)
{
}

template <
        typename affix_allocator_type>
inline auto& affix_allocator_helper<
        affix_allocator_type>::get_allocator()
{
    return instance_.allocator_;
}

template <
        typename allocator,
        typename prefix,
        typename suffix,
        bool verify>
inline memory_block affix_allocator<
        allocator,
        prefix,
        suffix,
        verify>::allocate(std::size_t size)
{
    auto block = allocator_.allocate(size_of_allocation(size));
    if (null_allocator_.owns(block))
        return block;

    construct_prefix(block);
    construct_suffix(block);

    return offset_block(block, size);
}

template <
        typename allocator,
        typename prefix,
        typename suffix,
        bool verify>
inline void affix_allocator<
        allocator,
        prefix,
        suffix,
        verify>::deallocate(memory_block& block)
{
    auto original = original_block(block);
    if (allocator_.owns(original))
    {
        if (verify)
        {
            verify_prefix(original);
            verify_suffix(original);
        }
        
        allocator_.deallocate(original);
        block = original;
    }
}

template <
        typename allocator,
        typename prefix,
        typename suffix,
        bool verify>
inline bool affix_allocator<
        allocator,
        prefix,
        suffix,
        verify>::owns(memory_block& block)
{
    auto original = original_block(block);
    return allocator_.owns(original);
}

template <
        typename allocator,
        typename prefix,
        typename suffix,
        bool verify>
inline std::size_t affix_allocator<
        allocator,
        prefix,
        suffix,
        verify>::size_of_allocation(std::size_t size) noexcept
{
    return size + size_of<prefix>() + size_of<suffix>();
}

template <
        typename allocator,
        typename prefix,
        typename suffix,
        bool verify>
inline memory_block affix_allocator<
        allocator,
        prefix,
        suffix,
        verify>::offset_block(memory_block& block, std::size_t size) noexcept
{
    return { block + size_of<prefix>(), size };
}

template <
        typename allocator,
        typename prefix,
        typename suffix,
        bool verify>
inline memory_block affix_allocator<
        allocator,
        prefix,
        suffix,
        verify>::original_block(memory_block& block) noexcept
{
    return { block - size_of<prefix>(), block.size + size_of<prefix>() + size_of<suffix>() };
}

template <
        typename allocator,
        typename prefix,
        typename suffix,
        bool verify>
inline void affix_allocator<
        allocator,
        prefix,
        suffix,
        verify>::construct_prefix(memory_block& block)
{
    construct_at<prefix>(block, 0ul);
}

template <
        typename allocator,
        typename prefix,
        typename suffix,
        bool verify>
inline void affix_allocator<
        allocator,
        prefix,
        suffix,
        verify>::construct_suffix(memory_block& block)
{
    construct_at<suffix>(block, block.size - size_of<suffix>());
}

template <
        typename allocator,
        typename prefix,
        typename suffix,
        bool verify>
inline void affix_allocator<
        allocator,
        prefix,
        suffix,
        verify>::verify_prefix(memory_block& block)
{
    destruct_at<prefix>(block, 0ul);
}

template <
        typename allocator,
        typename prefix,
        typename suffix,
        bool verify>
inline void affix_allocator<
        allocator,
        prefix,
        suffix,
        verify>::verify_suffix(memory_block& block)
{
    destruct_at<suffix>(block, block.size - size_of<suffix>());
}

} }
