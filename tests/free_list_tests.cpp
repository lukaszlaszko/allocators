#include <boost/memory/free_list.hpp>
#include <boost/memory/memory_block.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <iterator>
#include <vector>

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

TEST(free_list, allocate_under_min_size)
{
    using allocator_type = free_list<mock_allocator, 2, 8, 32, 16>;
    using helper_type = free_list_helper<allocator_type>;
    allocator_type allocator;
    helper_type helper(allocator);
    
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                    Invoke([&allocator](std::size_t rs) -> memory_block 
                    { 
                        return { &allocator, rs }; 
                    }));
                    
     auto block = allocator.allocate(4ul);
     ASSERT_EQ(block.address, nullptr);
     ASSERT_EQ(block.size, 0ul);
}

TEST(free_list, allocate_over_max_size)
{
    using allocator_type = free_list<mock_allocator, 2, 8, 32, 16>;
    using helper_type = free_list_helper<allocator_type>;
    allocator_type allocator;
    helper_type helper(allocator);
    
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                    Invoke([&allocator](std::size_t rs) -> memory_block 
                    { 
                        return { &allocator, rs }; 
                    }));
                    
     auto block = allocator.allocate(64ul);
     ASSERT_EQ(block.address, nullptr);
     ASSERT_EQ(block.size, 0ul);
}

TEST(free_list, allocate_proper_size_with_multibatch)
{
    using allocator_type = free_list<mock_allocator, 2, 8, 32, 16>;
    using helper_type = free_list_helper<allocator_type>;
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<array<uint8_t, 128>> blocks;
    blocks.reserve(5);
    
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                    Invoke([&blocks](std::size_t rs) -> memory_block
                    {
                        assert(blocks.size() < 5);
                        
                        blocks.emplace_back();
                        return { blocks.back().data(), rs };
                    }));
    EXPECT_CALL(helper.get_allocator(), allocate(Eq(32ul + helper_type::node_size)))
            .Times(2);
    
    auto block = allocator.allocate(16ul);
    ASSERT_EQ(block.address, blocks[0].data() + helper_type::node_size);
    ASSERT_EQ(block.size, 32ul);
    ASSERT_EQ(distance(helper.begin(), helper.end()), 1ul);
}

TEST(free_list, allocate_returned_block)
{
    using allocator_type = free_list<mock_allocator, 2, 8, 32, 16>;
    using helper_type = free_list_helper<allocator_type>;
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<array<uint8_t, 128>> blocks;
    blocks.reserve(5);
    
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                    Invoke([&blocks](std::size_t rs) -> memory_block
                    {
                        assert(blocks.size() < 5);
                        
                        blocks.emplace_back();
                        return { blocks.back().data(), rs };
                    }));
    ON_CALL(helper.get_allocator(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(true));
    EXPECT_CALL(helper.get_allocator(), allocate(Eq(32ul + helper_type::node_size)))
            .Times(2);
    EXPECT_CALL(helper.get_allocator(), deallocate(A<memory_block&>()))
            .Times(0);
                    
    auto block = allocator.allocate(16ul);
    ASSERT_EQ(block.address, blocks[0].data() + helper_type::node_size);
    ASSERT_EQ(block.size, 32ul);
    ASSERT_EQ(distance(helper.begin(), helper.end()), 1ul);
    
    allocator.deallocate(block);
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
    ASSERT_EQ(distance(helper.begin(), helper.end()), 2ul);
    
    block = allocator.allocate(16ul);
    ASSERT_EQ(block.address, blocks[0].data() + helper_type::node_size);
    ASSERT_EQ(block.size, 32ul);
    ASSERT_EQ(distance(helper.begin(), helper.end()), 1ul);
}

TEST(free_list, allocate_capacity_reached)
{
    using allocator_type = free_list<mock_allocator, 2, 8, 32, 2>;
    using helper_type = free_list_helper<allocator_type>;
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<array<uint8_t, 128>> blocks;
    blocks.reserve(5);
    
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                    Invoke([&blocks](std::size_t rs) -> memory_block
                    { 
                        assert(blocks.size() < 5);
                        
                        blocks.emplace_back();
                        return { blocks.back().data(), rs };
                    }));
    EXPECT_CALL(helper.get_allocator(), allocate(Eq(32ul + helper_type::node_size)))
            .Times(2);
    
    auto block_1 = allocator.allocate(16ul);
    ASSERT_NE(block_1.address, nullptr);
    ASSERT_NE(block_1.size, 0ul);
    ASSERT_EQ(distance(helper.begin(), helper.end()), 1ul);
    
    auto block_2 = allocator.allocate(16ul);
    ASSERT_NE(block_2.address, nullptr);
    ASSERT_NE(block_2.size, 0ul);
    ASSERT_EQ(distance(helper.begin(), helper.end()), 0ul);
    
    auto block_3 = allocator.allocate(16ul);
    ASSERT_EQ(block_3.address, nullptr);
    ASSERT_EQ(block_3.size, 0ul);
}

TEST(free_list, owns_under_min_size)
{
    using allocator_type = free_list<mock_allocator, 2, 8, 32, 2>;
    using helper_type = free_list_helper<allocator_type>;
    allocator_type allocator;
    helper_type helper(allocator);
    
    ON_CALL(helper.get_allocator(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(true));
    
    memory_block block { nullptr, 4ul };
    ASSERT_FALSE(allocator.owns(block));
}

TEST(free_list, owns_over_max_size)
{
    using allocator_type = free_list<mock_allocator, 2, 8, 32, 2>;
    using helper_type = free_list_helper<allocator_type>;
    allocator_type allocator;
    helper_type helper(allocator);
    
    ON_CALL(helper.get_allocator(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(true));
    
    memory_block block { nullptr, 64ul };
    ASSERT_FALSE(allocator.owns(block));
}

TEST(free_list, owns_proper_size_underlying_negative)
{
    using allocator_type = free_list<mock_allocator, 2, 8, 32, 2>;
    using helper_type = free_list_helper<allocator_type>;
    allocator_type allocator;
    helper_type helper(allocator);
    
    ON_CALL(helper.get_allocator(), owns(A<memory_block&>()))
            .WillByDefault(
                    Return(false));
    EXPECT_CALL(helper.get_allocator(), owns(A<memory_block&>()))
            .Times(1);
    
    memory_block block { nullptr, 16ul };
    ASSERT_FALSE(allocator.owns(block));
}

TEST(free_list, owns_proper_size_underlying_positive)
{
    using allocator_type = free_list<mock_allocator, 2, 8, 32, 2>;
    using helper_type = free_list_helper<allocator_type>;
    allocator_type allocator;
    helper_type helper(allocator);
    
    ON_CALL(helper.get_allocator(), owns(A<memory_block&>()))
            .WillByDefault(
                           Return(true));
    EXPECT_CALL(helper.get_allocator(), owns(A<memory_block&>()))
            .Times(1);
    
    memory_block block { nullptr, 16ul };
    ASSERT_TRUE(allocator.owns(block));
}
