#include <boost/memory/bitmapped_block.hpp>
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

TEST(bitmapped_allocator_bitmap, set)
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

TEST(bitmapped_allocator_bitmap, reset)
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

TEST(bitmapped_allocator, create)
{
    using allocator_type = bitmapped_block<mock_allocator, 0, 32, 4, 1>;
    using helper_type = bitmapped_block_helper<allocator_type>;
    
    //allocator.allocate();
}
