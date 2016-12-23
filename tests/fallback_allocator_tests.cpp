#include <boost/memory/fallback_allocator.hpp>
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

TEST(fallback_allocator, allocate_without_fallback)
{
    fallback_allocator<mock_allocator, mock_allocator> allocator;
    
    size_t requested_size;
    ON_CALL(allocator.get_primary(), allocate(A<size_t>()))
            .WillByDefault(
                    DoAll(
                        SaveArg<0>(&requested_size),
                        Return(memory_block { &allocator, requested_size })));
            
    EXPECT_CALL(allocator.get_primary(), allocate(A<size_t>()))
            .Times(1);            
    EXPECT_CALL(allocator.get_secondary(), allocate(A<size_t>()))
            .Times(0);
    
    auto block = allocator.allocate(10ul);
    
    ASSERT_NE(block.address, nullptr);
    ASSERT_NE(block.size, requested_size);
}

TEST(fallback_allocator, allocate_fallback)
{
    fallback_allocator<mock_allocator, mock_allocator> allocator;
    
    ON_CALL(allocator.get_primary(), allocate(A<size_t>()))
            .WillByDefault(
                    Return(memory_block { nullptr, 0ul }));
    
    size_t requested_size;
    ON_CALL(allocator.get_secondary(), allocate(A<size_t>()))
            .WillByDefault(
                    DoAll(
                        SaveArg<0>(&requested_size),
                        Return(memory_block { &allocator, requested_size })));
            
    EXPECT_CALL(allocator.get_primary(), allocate(A<size_t>()))
            .Times(1);            
    EXPECT_CALL(allocator.get_secondary(), allocate(A<size_t>()))
            .Times(1);
    
    auto block = allocator.allocate(10ul);
    
    ASSERT_NE(block.address, nullptr);
    ASSERT_NE(block.size, requested_size);
}

TEST(fallback_allocator, deallocate_owned_by_primary)
{
    fallback_allocator<mock_allocator, mock_allocator> allocator;
    
    ON_CALL(allocator.get_primary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(true));
    ON_CALL(allocator.get_secondary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(false));
    ON_CALL(allocator.get_primary(), deallocate(A<memory_block&>()))
            .WillByDefault(
                    Invoke([](auto& block)
                    {
                        block.address = nullptr;
                        block.size = 0ul;
                    }));
                    
    EXPECT_CALL(allocator.get_primary(), deallocate(A<memory_block&>()))
            .Times(1);            
    EXPECT_CALL(allocator.get_secondary(), deallocate(A<memory_block&>()))
            .Times(0);
                    
    memory_block block { &allocator, 10ul };
    allocator.deallocate(block);
    
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(fallback_allocator, deallocate_owned_by_secondary)
{
    fallback_allocator<mock_allocator, mock_allocator> allocator;
    
    ON_CALL(allocator.get_primary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(false));
    ON_CALL(allocator.get_secondary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(true));  
    ON_CALL(allocator.get_secondary(), deallocate(A<memory_block&>()))
            .WillByDefault(
                    Invoke([](auto& block) 
                    {
                        block.address = nullptr;
                        block.size = 0ul;
                    }));
                    
    EXPECT_CALL(allocator.get_primary(), deallocate(A<memory_block&>()))
            .Times(0);            
    EXPECT_CALL(allocator.get_secondary(), deallocate(A<memory_block&>()))
            .Times(1);
                    
    memory_block block { &allocator, 10ul };
    allocator.deallocate(block);
    
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(fallback_allocator, owns_primary)
{
    fallback_allocator<mock_allocator, mock_allocator> allocator;
    
    ON_CALL(allocator.get_primary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(true));
    ON_CALL(allocator.get_secondary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(false));
    
    EXPECT_CALL(allocator.get_primary(), owns(A<memory_block&>()))
            .Times(1);            
    EXPECT_CALL(allocator.get_secondary(), owns(A<memory_block&>()))
            .Times(0);
    
    memory_block block { &allocator, 10ul };
    ASSERT_TRUE(allocator.owns(block));
}

TEST(fallback_allocator, owns_secondary)
{
    fallback_allocator<mock_allocator, mock_allocator> allocator;
    
    ON_CALL(allocator.get_primary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(false));
    ON_CALL(allocator.get_secondary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(true));
    
    EXPECT_CALL(allocator.get_primary(), owns(A<memory_block&>()))
            .Times(1);            
    EXPECT_CALL(allocator.get_secondary(), owns(A<memory_block&>()))
            .Times(1);
    
    memory_block block { &allocator, 10ul };
    ASSERT_TRUE(allocator.owns(block));
}

TEST(fallback_allocator, owns_negative)
{
    fallback_allocator<mock_allocator, mock_allocator> allocator;
    
    ON_CALL(allocator.get_primary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(false));
    ON_CALL(allocator.get_secondary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(false));
    
    EXPECT_CALL(allocator.get_primary(), owns(A<memory_block&>()))
            .Times(1);            
    EXPECT_CALL(allocator.get_secondary(), owns(A<memory_block&>()))
            .Times(1);
    
    memory_block block { &allocator, 10ul };
    ASSERT_FALSE(allocator.owns(block));
}
