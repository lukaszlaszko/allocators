#include <boost/memory/segregator.hpp>
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

TEST(segregator, allocate_under_threashold)
{
    segregator<10ul, mock_allocator, mock_allocator> allocator;

    ON_CALL(allocator.get_primary(), allocate(A<size_t>()))
            .WillByDefault(
                    Invoke([&allocator](std::size_t rs) -> memory_block 
                    { 
                        return { &allocator, rs }; 
                    }));
    ON_CALL(allocator.get_secondary(), allocate(A<size_t>()))
            .WillByDefault(
                    Invoke([](std::size_t rs) -> memory_block 
                    { 
                        return { nullptr, 0ul }; 
                    }));

    EXPECT_CALL(allocator.get_primary(), allocate(A<size_t>()))
            .Times(1);
    
    auto block = allocator.allocate(5ul);
    ASSERT_EQ(block.address, &allocator);
    ASSERT_EQ(block.size, 5ul);
}

TEST(segregator, allocate_equal_threshold)
{
    segregator<10ul, mock_allocator, mock_allocator> allocator;

    ON_CALL(allocator.get_primary(), allocate(A<size_t>()))
            .WillByDefault(
                    Invoke([&allocator](std::size_t rs) -> memory_block 
                    { 
                        return { &allocator, rs }; 
                    }));
    ON_CALL(allocator.get_secondary(), allocate(A<size_t>()))
            .WillByDefault(
                    Invoke([](std::size_t rs) -> memory_block 
                    { 
                        return { nullptr, 0ul }; 
                    }));

    EXPECT_CALL(allocator.get_primary(), allocate(A<size_t>()))
            .Times(1);
    
    auto block = allocator.allocate(10ul);
    ASSERT_EQ(block.address, &allocator);
    ASSERT_EQ(block.size, 10ul);
}

TEST(segregator, allocate_over_threashold)
{
    segregator<10ul, mock_allocator, mock_allocator> allocator;

    ON_CALL(allocator.get_primary(), allocate(A<size_t>()))
            .WillByDefault(
                    Invoke([](std::size_t rs) -> memory_block 
                    { 
                        return {nullptr, 0ul }; 
                    }));
    ON_CALL(allocator.get_secondary(), allocate(A<size_t>()))
            .WillByDefault(
                    Invoke([&allocator](std::size_t rs) -> memory_block 
                    { 
                        return { &allocator, rs }; 
                    }));

    EXPECT_CALL(allocator.get_secondary(), allocate(A<size_t>()))
            .Times(1);
    
    auto block = allocator.allocate(11ul);
    ASSERT_EQ(block.address, &allocator);
    ASSERT_EQ(block.size, 11ul);
}

TEST(segregator, deallocate_under_threshold)
{
    segregator<10ul, mock_allocator, mock_allocator> allocator;

    ON_CALL(allocator.get_primary(), deallocate(A<memory_block&>()))
            .WillByDefault(
                    Invoke([](memory_block& b)
                    {
                        b.address = nullptr;
                        b.size = 0ul;
                    }));
    ON_CALL(allocator.get_secondary(), deallocate(A<memory_block&>()))
            .WillByDefault(
                    Invoke([&allocator](memory_block& b)
                    {  
                    }));

    EXPECT_CALL(allocator.get_primary(), deallocate(A<memory_block&>()))
            .Times(1);
    
    memory_block block { &allocator, 5ul };
    allocator.deallocate(block);
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(segregator, deallocate_equal_threshold)
{
    segregator<10ul, mock_allocator, mock_allocator> allocator;

    ON_CALL(allocator.get_primary(), deallocate(A<memory_block&>()))
            .WillByDefault(
                    Invoke([](memory_block& b)
                    {
                        b.address = nullptr;
                        b.size = 0ul;
                    }));
    ON_CALL(allocator.get_secondary(), deallocate(A<memory_block&>()))
            .WillByDefault(
                    Invoke([&allocator](memory_block& b)
                    {  
                    }));

    EXPECT_CALL(allocator.get_primary(), deallocate(A<memory_block&>()))
            .Times(1);
    
    memory_block block { &allocator, 10ul };
    allocator.deallocate(block);
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(segregator, deallocate_over_threshold)
{
    segregator<10ul, mock_allocator, mock_allocator> allocator;

    ON_CALL(allocator.get_primary(), deallocate(A<memory_block&>()))
            .WillByDefault(
                    Invoke([](memory_block& b)
                    {
                    }));
    ON_CALL(allocator.get_secondary(), deallocate(A<memory_block&>()))
            .WillByDefault(
                    Invoke([](memory_block& b)
                    {
                        b.address = nullptr;
                        b.size = 0ul;
                    }));

    EXPECT_CALL(allocator.get_secondary(), deallocate(A<memory_block&>()))
            .Times(1);
    
    memory_block block { &allocator, 12ul };
    allocator.deallocate(block);
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(segregator, owns_below_threshold)
{
    segregator<10ul, mock_allocator, mock_allocator> allocator;

    ON_CALL(allocator.get_primary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(true));
    ON_CALL(allocator.get_secondary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(false));

    EXPECT_CALL(allocator.get_primary(), owns(A<memory_block&>()))
            .Times(1);
    
    memory_block block { &allocator, 5ul };
    ASSERT_TRUE(allocator.owns(block));
}

TEST(segregator, owns_equal_threshold)
{
    segregator<10ul, mock_allocator, mock_allocator> allocator;

    ON_CALL(allocator.get_primary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(true));
    ON_CALL(allocator.get_secondary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(false));

    EXPECT_CALL(allocator.get_primary(), owns(A<memory_block&>()))
            .Times(1);
    
    memory_block block { &allocator, 10ul };
    ASSERT_TRUE(allocator.owns(block));
}

TEST(segregator, owns_over_threshold)
{
    segregator<10ul, mock_allocator, mock_allocator> allocator;

    ON_CALL(allocator.get_primary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(false));
    ON_CALL(allocator.get_secondary(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(true));

    EXPECT_CALL(allocator.get_secondary(), owns(A<memory_block&>()))
            .Times(1);
    
    memory_block block { &allocator, 12ul };
    ASSERT_TRUE(allocator.owns(block));
}
