#include <boost/memory/alignment.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace boost::memory;


TEST(alignment, align_forward_single_byte_alignment)
{
    std::uint8_t data[200];
    
    ASSERT_TRUE(align_forward<1>(data) == data);
}

TEST(alignment, align_forward_two_bytes_no_alignment)
{
    std::uint8_t data[200];
    std::uint8_t* aligned_address = reinterpret_cast<std::intptr_t>(data) % 2 == 0 
                                  ? data : data + 1;
    
    ASSERT_TRUE(align_forward<2>(aligned_address) == aligned_address);
}

TEST(alignment, align_forward_two_bytes_alignment)
{
    std::uint8_t data[200];
    std::uint8_t* non_aligned_address = reinterpret_cast<std::intptr_t>(data) % 2 != 0 
                                      ? data : data + 1;
    
    ASSERT_TRUE(align_forward<2>(non_aligned_address) == non_aligned_address + 1);
}

TEST(alignment, align_backward_single_byte_alignment)
{
    std::uint8_t data[200];
    
    ASSERT_TRUE(align_backward<1>(data) == data);
}

TEST(alignment, align_backward_two_bytes_no_alignment)
{
    std::uint8_t data[200];
    std::uint8_t* aligned_address = reinterpret_cast<std::intptr_t>(data) % 2 == 0 
                                  ? data : data + 1;
    
    ASSERT_TRUE(align_backward<2>(aligned_address) == aligned_address);
}

TEST(alignment, align_backward_two_bytes_alignment)
{
    std::uint8_t data[200];
    std::uint8_t* non_aligned_address = reinterpret_cast<std::intptr_t>(data) % 2 != 0 
                                      ? data : data + 1;
    
    ASSERT_TRUE(align_backward<2>(non_aligned_address) == non_aligned_address - 1);
}

