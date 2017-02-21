#include <boost/memory/memory_block.hpp>
#include <boost/memory/allocation_guard.hpp>
#include <boost/memory/stack_allocator.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>

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


TEST(memory_block_guard, create_destroy)
{
    using guard_type = allocation_guard<mock_allocator>;
    
    vector<array<uint8_t, 128>> blocks;
    blocks.reserve(1);
    
    mock_allocator allocator;
    ON_CALL(allocator, allocate(A<size_t>()))
            .WillByDefault(
                   Invoke([&blocks](std::size_t rs) -> memory_block
                          {
                              assert(blocks.size() < 5);
                              
                              blocks.emplace_back();
                              return { blocks.back().data(), rs };
                          }));
    ON_CALL(allocator, deallocate(A<memory_block&>()))
            .WillByDefault(
                   Invoke([](memory_block& block)
                          {
                              block = { nullptr, 0ul };
                          }));
    
    EXPECT_CALL(allocator, allocate(A<size_t>()))
            .Times(1);
    EXPECT_CALL(allocator, deallocate(A<memory_block&>()))
            .Times(1);
    
    {
        guard_type memory_block_guard(allocator, 100ul);

        memory_block block = memory_block_guard;
        ASSERT_EQ(block.address, blocks.back().data());
        ASSERT_EQ(block.size, 100ul);

        memory_block block1 = memory_block_guard;
        ASSERT_EQ(block1.address, block.address);
        ASSERT_EQ(block1.size, block.size);
    }
}

