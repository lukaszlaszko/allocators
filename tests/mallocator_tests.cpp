#include <boost/memory/mallocator.hpp>
#include <boost/memory/memory_block.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace boost::memory;


TEST(mallocator, allocate)
{
    mallocator allocator;
    
    auto block = allocator.allocate(10ul);
    ASSERT_NE(block.address, nullptr);
    ASSERT_EQ(block.size, 10ul);
}

TEST(mallocator, deallocate)
{
    mallocator allocator;
    
    auto block = allocator.allocate(10ul);
    ASSERT_NE(block.address, nullptr);
    ASSERT_EQ(block.size, 10ul);
    
    allocator.deallocate(block);
    
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(mallocator, owns)
{
    mallocator allocator;
    
    memory_block block_1 { nullptr, 0ul };
    ASSERT_FALSE(allocator.owns(block_1));
    
    memory_block block_2 { &allocator, 0ul };
    ASSERT_TRUE(allocator.owns(block_2));
}
