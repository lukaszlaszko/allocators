#pragma once

#include <type_traits>


namespace boost { namespace memory {

template <typename pointer_type>
inline buffer_ref::buffer_ref(
        pointer_type* pointer,
        size_t length)
    :
        data_(reinterpret_cast<void*>(pointer)),
        length_(length)
{

}

template <typename pointer_type>
inline buffer_ref::buffer_ref(
        const pointer_type* pointer,
        size_t length)
    :
        data_(reinterpret_cast<void*>(const_cast<pointer_type*>(pointer))),
        length_(length)
{

}

template <typename pointer_type>
inline buffer_ref::buffer_ref(pointer_type* pointer)
    :
        data_(reinterpret_cast<void*>(pointer)),
        length_(sizeof(pointer_type))
{

}

inline buffer_ref::buffer_ref(const memory_block& block)
    :
        data_(block.address),
        length_(block.size)
{

}

inline buffer_ref::buffer_ref(const std::string& s)
    :
        data_(reinterpret_cast<void*>(const_cast<char*>(s.data()))),
        length_(s.length())
{

}

template <typename reference_type>
inline reference_type buffer_ref::as()
{
    static_assert(std::is_reference<reference_type>::value, "must be a reference type!");

    using base_type = typename std::remove_reference<reference_type>::type;
    using pointer_type = typename std::add_pointer<base_type>::type;

    return *reinterpret_cast<pointer_type>(data_);
}

template <typename pointer_type>
inline pointer_type buffer_ref::as_pointer()
{
    static_assert(std::is_pointer<pointer_type>::value, "must be a pointer");

    return reinterpret_cast<pointer_type>(data_);
}

inline size_t buffer_ref::length() const
{
    return length_;
}

inline buffer_ref buffer_ref::subbuf(size_t length) const
{
    return buffer_ref(data_, length);
}

inline buffer_ref buffer_ref::subbuf(size_t offset, size_t length) const
{
    return buffer_ref(reinterpret_cast<uint8_t*>(data_) + offset, length);
}

} }

