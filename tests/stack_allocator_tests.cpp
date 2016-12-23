#include <boost/memory/stack_allocator.hpp>
#include <boost/memory/memory_block.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace boost::memory;


TEST(stack_allocator, allocate_success)
{
    stack_allocator<4096> allocator;
    
    auto block = allocator.allocate(10ul);
    ASSERT_EQ(block.address, &allocator);
    ASSERT_EQ(block.size, 10ul);
}

TEST(stack_allocator, allocate_aligned_success)
{
    stack_allocator<4096, 8> allocator;
    
    auto block_1 = allocator.allocate(10ul);
    ASSERT_EQ(block_1.address, &allocator);
    ASSERT_EQ(block_1.size, 10ul);
    
    auto block_2 = allocator.allocate(10ul);
    ASSERT_EQ(block_2.address, block_1 + 16);
    ASSERT_EQ(block_2.size, 10ul);
}

TEST(stack_allocator, allocate_full)
{
    stack_allocator<4096> allocator;
    
    {
        auto block = allocator.allocate(4096ul);
        ASSERT_EQ(block.address, &allocator);
        ASSERT_EQ(block.size, 4096ul);
    }
    
    {
        auto block = allocator.allocate(1ul);
        ASSERT_EQ(block.address, nullptr);
        ASSERT_EQ(block.size, 0ul);
    }
}

TEST(stack_allocator, allocate_exceed)
{
    stack_allocator<4096> allocator;
    
    auto block = allocator.allocate(4097ul);
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(stack_allocator, deallocate_last)
{
    stack_allocator<4096> allocator;
    
    auto block_1 = allocator.allocate(10ul);
    ASSERT_EQ(block_1.address, &allocator);
    ASSERT_EQ(block_1.size, 10ul);
    
    auto block_2 = allocator.allocate(5ul);
    ASSERT_EQ(block_2.address, reinterpret_cast<uint8_t*>(block_1.address) + block_1.size);
    ASSERT_EQ(block_2.size, 5ul);
    
    allocator.deallocate(block_1);
    ASSERT_EQ(block_1.address, &allocator);
    ASSERT_EQ(block_1.size, 10ul);
    
    allocator.deallocate(block_2);
    ASSERT_EQ(block_2.address, nullptr);
    ASSERT_EQ(block_2.size, 0ul);
    
    auto block_3 = allocator.allocate(12ul);
    ASSERT_EQ(block_3.address, reinterpret_cast<uint8_t*>(block_1.address) + block_1.size);
    ASSERT_EQ(block_3.size, 12ul);
}

TEST(stack_allocator, deallocate_last_aligned)
{
    stack_allocator<4096, 8> allocator;
    
    auto block_1 = allocator.allocate(10ul);
    ASSERT_EQ(block_1.address, &allocator);
    ASSERT_EQ(block_1.size, 10ul);
    
    auto block_2 = allocator.allocate(5ul);
    ASSERT_EQ(block_2.address, block_1 + 16);
    ASSERT_EQ(block_2.size, 5ul);
    
    allocator.deallocate(block_1);
    ASSERT_EQ(block_1.address, &allocator);
    ASSERT_EQ(block_1.size, 10ul);
    
    allocator.deallocate(block_2);
    ASSERT_EQ(block_2.address, nullptr);
    ASSERT_EQ(block_2.size, 0ul);
    
    auto block_3 = allocator.allocate(12ul);
    ASSERT_EQ(block_3.address, block_1 + 16);
    ASSERT_EQ(block_3.size, 12ul);
}

TEST(stack_allocator, owns_positive)
{
    stack_allocator<4096> allocator;
    
    memory_block block { &allocator + 100ul, 50ul };
    ASSERT_TRUE(allocator.owns(block));
}

TEST(stack_allocator, owns_negative)
{
    stack_allocator<4096> allocator;
    
    memory_block block { &allocator + 5000ul, 50ul };
    ASSERT_FALSE(allocator.owns(block));
}

