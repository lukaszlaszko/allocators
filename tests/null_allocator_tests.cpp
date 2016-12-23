#include <boost/memory/null_allocator.hpp>

#include <gtest/gtest.h>

using namespace boost::memory;
       
        
TEST(null_allocator, allocate)
{
    null_allocator allocator;
    auto block = allocator.allocate(10ul);
    
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(null_allocator, deallocate)
{
    null_allocator allocator;
    
    auto block = allocator.allocate(10ul);
    allocator.deallocate(block);
    
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(null_allocator, owns_positive)
{
    null_allocator allocator;
    
    memory_block block_1 { nullptr, 0ul };
    ASSERT_TRUE(allocator.owns(block_1));
    
    memory_block block_2 { nullptr, 10ul };
    ASSERT_TRUE(allocator.owns(block_2));
}

TEST(null_allocator, owns_negative)
{
    null_allocator allocator;
    
    memory_block block_1 { &allocator, 0ul };
    ASSERT_FALSE(allocator.owns(block_1));
}

