#include <boost/memory/affix_allocator.hpp>
#include <boost/memory/memory_block.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
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

TEST(affix_allocator, allocate_with_prefix)
{
    struct prefix
    {
        uint8_t marker[5] { 0, 1, 2, 3, 4 };
    };

    using allocator_type = affix_allocator<mock_allocator, prefix>;
    using helper_type = affix_allocator_helper<allocator_type>;
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
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);

    auto block = allocator.allocate(16ul);
    ASSERT_EQ(block.address, blocks[0].data() + sizeof(prefix));
    ASSERT_EQ(block.size, 16ul);

    auto subarray = vector<uint8_t>(blocks[0].begin(), blocks[0].begin() + 5);
    ASSERT_THAT(subarray, ElementsAre(0, 1, 2, 3, 4));
}

TEST(affix_allocator, allocate_with_suffix)
{
    struct suffix
    {
        uint8_t marker[5] { 0, 1, 2, 3, 4 };
    };

    using allocator_type = affix_allocator<mock_allocator, void, suffix>;
    using helper_type = affix_allocator_helper<allocator_type>;
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
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);

    auto block = allocator.allocate(16ul);
    ASSERT_EQ(block.address, blocks[0].data());
    ASSERT_EQ(block.size, 16ul);

    auto subarray = vector<uint8_t>(blocks[0].begin() + 16ul, blocks[0].begin() + 21ul);
    ASSERT_THAT(subarray, ElementsAre(0, 1, 2, 3, 4));
}

TEST(affix_allocator, owns_with_prefix)
{
    struct prefix
    {
        uint8_t marker[5] { 0, 1, 2, 3, 4 };
    };

    using allocator_type = affix_allocator<mock_allocator, prefix>;
    using helper_type = affix_allocator_helper<allocator_type>;
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

    vector<memory_block> owned_blocks;
    ON_CALL(helper.get_allocator(), owns(A<memory_block&>()))
            .WillByDefault(
                    Invoke([&owned_blocks](memory_block& block) -> bool
                    {
                        owned_blocks.push_back(block);
                        return true;
                    }));
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);
    EXPECT_CALL(helper.get_allocator(), owns(A<memory_block&>()))
            .Times(1);

    auto block = allocator.allocate(16ul);
    ASSERT_EQ(block.address, blocks[0].data() + sizeof(prefix));
    ASSERT_EQ(block.size, 16ul);

    ASSERT_TRUE(allocator.owns(block));
    ASSERT_EQ(owned_blocks.size(), 1ul);
    ASSERT_EQ(owned_blocks[0].address, blocks[0].data());
    ASSERT_EQ(owned_blocks[0].size, 16ul + sizeof(prefix));
}

TEST(affix_allocator, owns_without_prefix)
{
    using allocator_type = affix_allocator<mock_allocator, void>;
    using helper_type = affix_allocator_helper<allocator_type>;
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

    vector<memory_block> owned_blocks;
    ON_CALL(helper.get_allocator(), owns(A<memory_block&>()))
            .WillByDefault(
                    Invoke([&owned_blocks](memory_block& block) -> bool
                    {
                        owned_blocks.push_back(block);
                        return true;
                    }));
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);
    EXPECT_CALL(helper.get_allocator(), owns(A<memory_block&>()))
            .Times(1);

    auto block = allocator.allocate(16ul);
    ASSERT_EQ(block.address, blocks[0].data());
    ASSERT_EQ(block.size, 16ul);

    ASSERT_TRUE(allocator.owns(block));
    ASSERT_EQ(owned_blocks.size(), 1ul);
    ASSERT_EQ(owned_blocks[0].address, blocks[0].data());
    ASSERT_EQ(owned_blocks[0].size, 16ul);
}

TEST(affix_allocator, deallocate_without_verify)
{
    using allocator_type = affix_allocator<mock_allocator, void>;
    using helper_type = affix_allocator_helper<allocator_type>;
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
    vector<memory_block> deallocated_blocks;
    ON_CALL(helper.get_allocator(), deallocate(A<memory_block&>()))
            .WillByDefault(
                    Invoke([&deallocated_blocks](memory_block& block)
                    {
                        deallocated_blocks.push_back(block);
                        block = { nullptr, 0ul };
                    }));
    
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);
    EXPECT_CALL(helper.get_allocator(), owns(A<memory_block&>()))
            .Times(1);
    EXPECT_CALL(helper.get_allocator(), deallocate(A<memory_block&>()))
            .Times(1);

    auto block = allocator.allocate(16ul);
    ASSERT_EQ(block.address, blocks[0].data());
    ASSERT_EQ(block.size, 16ul);
    
    allocator.deallocate(block);
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
    ASSERT_EQ(deallocated_blocks.size(), 1ul);
    ASSERT_EQ(deallocated_blocks[0].address, blocks[0].data());
    ASSERT_EQ(deallocated_blocks[0].size, 16ul);
}

TEST(affix_allocator, deallocate_with_verify_prefix)
{
    static auto constructed_count = 0ul;
    static auto destroyed_count = 0ul;
    struct prefix
    {
        prefix()
        {
            constructed_count++;
        }
        
        ~prefix()
        {
            destroyed_count++;
        }
    };
    
    using allocator_type = affix_allocator<mock_allocator, prefix, void_t, true>;
    using helper_type = affix_allocator_helper<allocator_type>;
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
    vector<memory_block> deallocated_blocks;
    ON_CALL(helper.get_allocator(), deallocate(A<memory_block&>()))
            .WillByDefault(
                    Invoke([&deallocated_blocks](memory_block& block)
                    {
                        deallocated_blocks.push_back(block);
                        block = { nullptr, 0ul };
                    }));
    
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);
    EXPECT_CALL(helper.get_allocator(), owns(A<memory_block&>()))
            .Times(1);
    EXPECT_CALL(helper.get_allocator(), deallocate(A<memory_block&>()))
            .Times(1);

    auto block = allocator.allocate(16ul);
    ASSERT_EQ(block.address, blocks[0].data() + sizeof(prefix));
    ASSERT_EQ(block.size, 16ul);
    
    allocator.deallocate(block);
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
    ASSERT_EQ(deallocated_blocks.size(), 1ul);
    ASSERT_EQ(deallocated_blocks[0].address, blocks[0].data());
    ASSERT_EQ(deallocated_blocks[0].size, 16ul + sizeof(prefix));
    ASSERT_EQ(constructed_count, 1ul);
    ASSERT_EQ(destroyed_count, 1ul);
}

TEST(affix_allocator, deallocate_with_verify_suffix)
{

}

