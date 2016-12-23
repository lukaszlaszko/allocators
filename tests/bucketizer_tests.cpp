#include <boost/memory/bucketizer.hpp>
#include <boost/memory/memory_block.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace boost::memory;
using namespace testing;


class mock_allocator
{
public:
    MOCK_METHOD1(allocate, memory_block(size_t size));
    MOCK_METHOD1(deallocate, void(memory_block& block));
    MOCK_METHOD1(owns, bool(memory_block& block));
};

TEST(bucketizer, allocate_successful)
{
    bucketizer<mock_allocator, 4, 20, 5> allocator;
    
    ON_CALL(allocator.get<1>(), allocate(A<size_t>()))
            .WillByDefault(
                    Invoke([&allocator](std::size_t rs) -> memory_block 
                    { 
                        return { &allocator, rs }; 
                    }));

    EXPECT_CALL(allocator.get<1>(), allocate(A<size_t>()))
            .Times(1);
    
    auto block = allocator.allocate(11ul);
    ASSERT_EQ(block.address, &allocator);
    ASSERT_EQ(block.size, 11ul);
}

TEST(bucketizer, allocate_under_range)
{
    bucketizer<mock_allocator, 4, 20, 5> allocator;
    
    auto block = allocator.allocate(3ul);
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(bucketizer, allocate_over_range)
{
    bucketizer<mock_allocator, 4, 20, 5> allocator;
    
    auto block = allocator.allocate(22ul);
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(bucketizer, deallocate_successful)
{
    bucketizer<mock_allocator, 4, 20, 5> allocator;
    
    ON_CALL(allocator.get<1>(), deallocate(A<memory_block&>()))
            .WillByDefault(
                    Invoke([&allocator](memory_block& mb) 
                    { 
                        mb = { nullptr, 0ul }; 
                    }));

    EXPECT_CALL(allocator.get<1>(), deallocate(A<memory_block&>()))
            .Times(1);
    
    memory_block block { &allocator, 11ul };
    allocator.deallocate(block);
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(bucketizer, deallocate_under_range)
{
    bucketizer<mock_allocator, 4, 20, 5> allocator;
    
    memory_block block { &allocator, 2ul };
    allocator.deallocate(block);
    ASSERT_NE(block.address, nullptr);
    ASSERT_NE(block.size, 0ul);
}

TEST(bucketizer, deallocate_over_range)
{
    bucketizer<mock_allocator, 4, 20, 5> allocator;
    
    memory_block block { &allocator, 22ul };
    allocator.deallocate(block);
    ASSERT_NE(block.address, nullptr);
    ASSERT_NE(block.size, 0ul);
}

TEST(bucketizer, owns_successful)
{
    bucketizer<mock_allocator, 4, 20, 5> allocator;
    
    ON_CALL(allocator.get<1>(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(true));

    EXPECT_CALL(allocator.get<1>(), owns(A<memory_block&>()))
            .Times(1);
    
    memory_block block { &allocator, 11ul };
    ASSERT_TRUE(allocator.owns(block));
}

TEST(bucketizer, owns_under_range)
{
    bucketizer<mock_allocator, 4, 20, 5> allocator;
    
    memory_block block { &allocator, 2ul };
    ASSERT_FALSE(allocator.owns(block));
}

TEST(bucketizer, owns_over_range)
{
    bucketizer<mock_allocator, 4, 20, 5> allocator;
    
    memory_block block { &allocator, 22ul };
    ASSERT_FALSE(allocator.owns(block));
}
