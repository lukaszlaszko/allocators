#include <boost/memory/buffer_ref.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>

using namespace std;
using namespace testing;
using namespace boost::memory;

TEST(buffer_ref, create_from_buffer)
{
    array<uint8_t, 12> data;
    buffer_ref data_ref(data.data(), data.size());
    
    ASSERT_EQ(data_ref.as_pointer<uint8_t*>(), data.data());
    ASSERT_EQ(data_ref.length(), data.size());
}

TEST(buffer_ref, create_from_pointer)
{
    struct entity
    {
        double field_1;
        char field_2[10];
    };
    
    entity data{};
    buffer_ref data_ref(&data);
    
    ASSERT_EQ(data_ref.as_pointer<uint8_t*>(), reinterpret_cast<uint8_t*>(&data));
    ASSERT_EQ(data_ref.length(), sizeof(entity));
}

TEST(buffer_ref, create_from_reference)
{
    struct entity
    {
        double field_1;
        char field_2[10];
    };
    
    entity data{};
    buffer_ref data_ref(data);
    
    ASSERT_EQ(data_ref.as_pointer<uint8_t*>(), reinterpret_cast<uint8_t*>(&data));
    ASSERT_EQ(data_ref.length(), sizeof(entity));
}

TEST(buffer_ref, as)
{
    struct entity
    {
        uint64_t field_1;
        double field_2;
    };
    entity data{};
    buffer_ref data_ref(data);
    
    data_ref.as<entity&>().field_1 = 103ul;
    data_ref.as<entity&>().field_2 = 89.0;
    
    ASSERT_EQ(data_ref.as<entity&>().field_1, data.field_1);
    ASSERT_EQ(data_ref.as<entity&>().field_2, data.field_2);
}

TEST(buffer_ref, as_pointer)
{
    string data("abcd1234");
    buffer_ref data_ref(data.data(), data.length());
    
    ASSERT_EQ(memcmp(data_ref.as_pointer<char*>(), data.data(), data.length()), 0);
}