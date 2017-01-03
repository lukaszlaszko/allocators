#include <boost/memory/bitmapped_block.hpp>
#include <boost/memory/memory_block.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

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

TEST(bitmapped_block_bitmap, set)
{
    using allocator_type = bitmapped_block<mock_allocator, 0, 32, 16, 1>;
    using bitmap_type = allocator_type::bitmap_type;
    using helper_type = bitmap_helper<bitmap_type>;
    
    bitmap_type bitmap;
    helper_type helper(bitmap);
    
    ASSERT_EQ(helper.flags_count(), 2);
    ASSERT_EQ(helper.get_flag(0ul), 0b00000000);
    ASSERT_EQ(helper.get_flag(1ul), 0b00000000);
    
    ASSERT_FALSE(bitmap.is_set(0ul));
    bitmap.set(0ul);
    ASSERT_TRUE(bitmap.is_set(0ul));
    
    ASSERT_FALSE(bitmap.is_set(1ul));
    bitmap.set(1ul);
    ASSERT_TRUE(bitmap.is_set(1ul));
    
    ASSERT_FALSE(bitmap.is_set(7ul));
    bitmap.set(7ul);
    ASSERT_TRUE(bitmap.is_set(7ul));
    
    ASSERT_FALSE(bitmap.is_set(8ul));
    bitmap.set(8ul);
    ASSERT_TRUE(bitmap.is_set(8ul));
}

TEST(bitmapped_block_bitmap, reset)
{
    using allocator_type = bitmapped_block<mock_allocator, 0, 32, 16, 1>;
    using bitmap_type = allocator_type::bitmap_type;
    using helper_type = bitmap_helper<bitmap_type>;
    
    bitmap_type bitmap;
    helper_type helper(bitmap);
    
    helper.get_flag(0ul) = 0b11111111;
    helper.get_flag(1ul) = 0b11111111;
    
    ASSERT_EQ(helper.flags_count(), 2);
    ASSERT_EQ(helper.get_flag(0ul), 0b11111111);
    ASSERT_EQ(helper.get_flag(1ul), 0b11111111);
    
    ASSERT_TRUE(bitmap.is_set(0ul));
    bitmap.reset(0ul);
    ASSERT_FALSE(bitmap.is_set(0ul));
    
    ASSERT_TRUE(bitmap.is_set(1ul));
    bitmap.reset(1ul);
    ASSERT_FALSE(bitmap.is_set(1ul));
    
    ASSERT_TRUE(bitmap.is_set(7ul));
    bitmap.reset(7ul);
    ASSERT_FALSE(bitmap.is_set(7ul));
    
    ASSERT_TRUE(bitmap.is_set(8ul));
    bitmap.reset(8ul);
    ASSERT_FALSE(bitmap.is_set(8ul));
}

TEST(bitmapped_block_bitmap, claim_success)
{
    using allocator_type = bitmapped_block<mock_allocator, 0, 32, 4, 1>;
    using bitmap_type = allocator_type::bitmap_type;
    using helper_type = bitmap_helper<bitmap_type>;
    
    bitmap_type bitmap;
    helper_type helper(bitmap);
    
    helper.get_flag(0ul) = 0b11111011;
    helper.get_flag(1ul) = 0b11111111;
    
    size_t index = 0;
    ASSERT_TRUE(bitmap.claim(index));
    ASSERT_EQ(index, 5ul);
    ASSERT_EQ(helper.get_flag(1) & 0b11111111, 0b11111111);
}

TEST(bitmapped_block_bitmap, claim_failure)
{
    using allocator_type = bitmapped_block<mock_allocator, 0, 32, 4, 1>;
    using bitmap_type = allocator_type::bitmap_type;
    using helper_type = bitmap_helper<bitmap_type>;
    
    bitmap_type bitmap;
    helper_type helper(bitmap);
    
    helper.get_flag(0ul) = 0b11111111;
    helper.get_flag(1ul) = 0b11111111;
    
    size_t index = 0;
    ASSERT_FALSE(bitmap.claim(index));
    ASSERT_EQ(helper.get_flag(1) & 0b11111111, 0b11111111);
}

TEST(bitmapped_block, allocate_under_min)
{
    using allocator_type = bitmapped_block<mock_allocator, 8ul, 32ul, 4ul>;
    using helper_type = bitmapped_block_helper<allocator_type>;
    
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<uint8_t> data(1024, 0);
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                    Return(memory_block { data.data(), 1024 }));
    
    auto block = allocator.allocate(4ul);
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(bitmapped_block, allocate_over_max)
{
    using allocator_type = bitmapped_block<mock_allocator, 8ul, 32ul, 4ul>;
    using helper_type = bitmapped_block_helper<allocator_type>;
    
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<uint8_t> data(1024, 0);
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                    Return(memory_block { data.data(), 1024 }));
    
    auto block = allocator.allocate(33ul);
    ASSERT_EQ(block.address, nullptr);
    ASSERT_EQ(block.size, 0ul);
}

TEST(bitmapped_block, allocate_no_alignment)
{
    using allocator_type = bitmapped_block<mock_allocator, 8ul, 32ul, 4ul, NO_ALIGNMENT>;
    using helper_type = bitmapped_block_helper<allocator_type>;
    
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<uint8_t> data;
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                    Invoke([&data](size_t size) -> memory_block
                    {
                        data.reserve(size);
                        return { data.data(), size };
                    }));
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);
    
    auto block = allocator.allocate(20ul);
    ASSERT_EQ(block.address, data.data() + sizeof(allocator_type::bitmap_type));
    ASSERT_EQ(block.size, 32ul);
    ASSERT_EQ(data.capacity(), 32ul * 4ul + sizeof(allocator_type::bitmap_type));
}

TEST(bitmapped_block, allocate_word_alignment)
{
    using allocator_type = bitmapped_block<mock_allocator, 8ul, 32ul, 4ul, WORD_ALIGNMENT>;
    using helper_type = bitmapped_block_helper<allocator_type>;
    
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<uint8_t> data;
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                   Invoke([&data](size_t size) -> memory_block
                          {
                              data.reserve(size);
                              return { data.data(), size };
                          }));
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);
    
    auto block_1 = allocator.allocate(20ul);
    ASSERT_NE(block_1.address, data.data() + sizeof(allocator_type::bitmap_type));
    ASSERT_EQ(block_1.address, align_forward<WORD_ALIGNMENT>(data.data() + sizeof(allocator_type::bitmap_type)));
    ASSERT_EQ(block_1.size, 32ul);
    
    auto block_2 = allocator.allocate(20ul);
    ASSERT_EQ(block_2.address, align_forward<WORD_ALIGNMENT>(data.data() + sizeof(allocator_type::bitmap_type)) + 32ul);
    ASSERT_EQ(block_2.size, 32ul);
}

TEST(bitmapped_block, allocate_failed)
{
    using allocator_type = bitmapped_block<mock_allocator, 8ul, 32ul, 4ul, NO_ALIGNMENT>;
    using helper_type = bitmapped_block_helper<allocator_type>;
    
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<uint8_t> data;
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                   Invoke([&data](size_t size) -> memory_block
                          {
                              data.reserve(size);
                              return { data.data(), size };
                          }));
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);

    // allocate all 4 blocks first
    for (auto i = 0_ui8; i < 4_ui8; i++)
    {
        auto block = allocator.allocate(20ul);
        ASSERT_NE(block.address, nullptr);
        ASSERT_NE(block.size, 0ul);
    }
    
    // failed allocation as the block is exhausted
    auto block_failed = allocator.allocate(20ul);
    ASSERT_EQ(block_failed.address, nullptr);
    ASSERT_EQ(block_failed.size, 0ul);
}

TEST(bitmapped_block, owns_no_alignment)
{
    using allocator_type = bitmapped_block<mock_allocator, 8ul, 32ul, 4ul, NO_ALIGNMENT>;
    using helper_type = bitmapped_block_helper<allocator_type>;
    
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<uint8_t> data;
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                   Invoke([&data](size_t size) -> memory_block
                          {
                              data.reserve(size);
                              return { data.data(), size };
                          }));
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);
    
    auto first_block = allocator.allocate(20ul);
    ASSERT_NE(first_block.address, nullptr);
    ASSERT_NE(first_block.size, 0ul);
    
    auto second_block = allocator.allocate(20ul);
    ASSERT_NE(second_block.address, nullptr);
    ASSERT_NE(second_block.size, 0ul);
    
    ASSERT_TRUE(allocator.owns(first_block));
    ASSERT_TRUE(allocator.owns(second_block));
}

TEST(bitmapped_block, owns_alignment)
{
    using allocator_type = bitmapped_block<mock_allocator, 8ul, 32ul, 4ul, WORD_ALIGNMENT>;
    using helper_type = bitmapped_block_helper<allocator_type>;
    
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<uint8_t> data;
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                   Invoke([&data](size_t size) -> memory_block
                          {
                              data.reserve(size);
                              return { data.data(), size };
                          }));
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);
    
    auto first_block = allocator.allocate(20ul);
    ASSERT_NE(first_block.address, nullptr);
    ASSERT_NE(first_block.size, 0ul);
    
    auto second_block = allocator.allocate(20ul);
    ASSERT_NE(second_block.address, nullptr);
    ASSERT_NE(second_block.size, 0ul);
    
    ASSERT_TRUE(allocator.owns(first_block));
    ASSERT_TRUE(allocator.owns(second_block));
}

TEST(bitmapped_block, owns_wrong_pointer)
{
    using allocator_type = bitmapped_block<mock_allocator, 8ul, 32ul, 4ul, NO_ALIGNMENT>;
    using helper_type = bitmapped_block_helper<allocator_type>;
    
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<uint8_t> data;
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                   Invoke([&data](size_t size) -> memory_block
                          {
                              data.reserve(size);
                              return { data.data(), size };
                          }));
    
    memory_block block_1 { data.data(), 32ul };
    ASSERT_FALSE(allocator.owns(block_1));
    
    memory_block block_2 { data.data() + data.size(), 32ul };
    ASSERT_FALSE(allocator.owns(block_2));
}

TEST(bitmapped_block, owns_inaccurate_pointer)
{
    using allocator_type = bitmapped_block<mock_allocator, 8ul, 32ul, 4ul, NO_ALIGNMENT>;
    using helper_type = bitmapped_block_helper<allocator_type>;
    
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<uint8_t> data;
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                   Invoke([&data](size_t size) -> memory_block
                          {
                              data.reserve(size);
                              return { data.data(), size };
                          }));
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);
    
    auto first_block = allocator.allocate(20ul);
    ASSERT_NE(first_block.address, nullptr);
    ASSERT_NE(first_block.size, 0ul);
    
    auto second_block = allocator.allocate(20ul);
    ASSERT_NE(second_block.address, nullptr);
    ASSERT_NE(second_block.size, 0ul);
    
    auto adjusted_block = first_block;
    adjusted_block.address = first_block - 10ul;
    ASSERT_FALSE(allocator.owns(adjusted_block));
    
    adjusted_block = second_block;
    second_block.address = second_block + 1ul;
    ASSERT_TRUE(allocator.owns(adjusted_block)); // address is accurate but within the range!
}

TEST(bitmapped_block, deallocate_first)
{
    using allocator_type = bitmapped_block<mock_allocator, 8ul, 32ul, 4ul, NO_ALIGNMENT>;
    using helper_type = bitmapped_block_helper<allocator_type>;
    
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<uint8_t> data;
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                   Invoke([&data](size_t size) -> memory_block
                          {
                              data.reserve(size);
                              return { data.data(), size };
                          }));
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);
    
    auto first_block = allocator.allocate(20ul);
    ASSERT_NE(first_block.address, nullptr);
    ASSERT_NE(first_block.size, 0ul);
    
    auto original_block = first_block;
    allocator.deallocate(first_block);
    ASSERT_EQ(first_block.address, nullptr);
    ASSERT_EQ(first_block.size, 0ul);
    
    auto reclaimed_block = allocator.allocate(25ul);
    ASSERT_EQ(original_block.address, reclaimed_block.address);
    ASSERT_EQ(original_block.size, reclaimed_block.size);
}

TEST(bitmapped_block, deallocate_middle)
{
    using allocator_type = bitmapped_block<mock_allocator, 8ul, 32ul, 4ul, NO_ALIGNMENT>;
    using helper_type = bitmapped_block_helper<allocator_type>;
    
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<uint8_t> data;
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                   Invoke([&data](size_t size) -> memory_block
                          {
                              data.reserve(size);
                              return { data.data(), size };
                          }));
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);
    
    auto first_block = allocator.allocate(20ul);
    ASSERT_NE(first_block.address, nullptr);
    ASSERT_NE(first_block.size, 0ul);
    
    auto second_block = allocator.allocate(20ul);
    ASSERT_NE(second_block.address, nullptr);
    ASSERT_NE(second_block.size, 0ul);
    
    auto third_block = allocator.allocate(20ul);
    ASSERT_NE(third_block.address, nullptr);
    ASSERT_NE(third_block.size, 0ul);
    
    auto original_block = second_block;
    allocator.deallocate(second_block);
    ASSERT_EQ(second_block.address, nullptr);
    ASSERT_EQ(second_block.size, 0ul);
    
    auto reclaimed_block = allocator.allocate(25ul);
    ASSERT_EQ(original_block.address, reclaimed_block.address);
    ASSERT_EQ(original_block.size, reclaimed_block.size);
}

TEST(bitmapped_block, deallocate_last)
{
    using allocator_type = bitmapped_block<mock_allocator, 8ul, 32ul, 4ul, NO_ALIGNMENT>;
    using helper_type = bitmapped_block_helper<allocator_type>;
    
    allocator_type allocator;
    helper_type helper(allocator);
    
    vector<uint8_t> data;
    ON_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .WillByDefault(
                   Invoke([&data](size_t size) -> memory_block
                          {
                              data.reserve(size);
                              return { data.data(), size };
                          }));
    EXPECT_CALL(helper.get_allocator(), allocate(A<size_t>()))
            .Times(1);
    
    auto first_block = allocator.allocate(20ul);
    ASSERT_NE(first_block.address, nullptr);
    ASSERT_NE(first_block.size, 0ul);
    
    auto second_block = allocator.allocate(20ul);
    ASSERT_NE(second_block.address, nullptr);
    ASSERT_NE(second_block.size, 0ul);
    
    auto third_block = allocator.allocate(20ul);
    ASSERT_NE(third_block.address, nullptr);
    ASSERT_NE(third_block.size, 0ul);
    
    auto fourth_block = allocator.allocate(20ul);
    ASSERT_NE(fourth_block.address, nullptr);
    ASSERT_NE(fourth_block.size, 0ul);
    
    auto original_block = fourth_block;
    allocator.deallocate(fourth_block);
    ASSERT_EQ(fourth_block.address, nullptr);
    ASSERT_EQ(fourth_block.size, 0ul);
    
    auto reclaimed_block = allocator.allocate(25ul);
    ASSERT_EQ(original_block.address, reclaimed_block.address);
    ASSERT_EQ(original_block.size, reclaimed_block.size);
}
