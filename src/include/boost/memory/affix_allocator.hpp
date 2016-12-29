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

template <typename affix_allocator_type>
class affix_allocator_helper
{
public:
    affix_allocator_helper(affix_allocator_type& instance);

    auto& get_allocator();

private:
    affix_allocator_type& instance_;
};

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

    memory_block allocate(std::size_t size);
    void deallocate(memory_block& block);

    bool owns(memory_block& block);

private:
    using this_affix_allocator_type = affix_allocator<allocator, prefix, suffix>;
    friend affix_allocator_helper<this_affix_allocator_type>;

    static std::size_t size_of_allocation(std::size_t size) noexcept;
    static memory_block offset_block(memory_block& block, std::size_t size) noexcept;
    static memory_block original_block(memory_block& block) noexcept;
    static void construct_prefix(memory_block& block);
    static void construct_suffix(memory_block& block);
    static void verify_prefix(memory_block& block);
    static void verify_suffix(memory_block& block);

    allocator allocator_;
    null_allocator null_allocator_;
};

} }

#include "affix_allocator.ipp"

