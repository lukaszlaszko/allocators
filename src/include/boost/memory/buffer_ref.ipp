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

template <typename reference_type>
inline buffer_ref::buffer_ref(reference_type& reference)
    :
        data_(reinterpret_cast<void*>(&reference)),
        length_(sizeof(reference_type))
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
    
} }

