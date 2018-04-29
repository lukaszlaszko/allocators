#pragma once

#include <boost/memory/memory_block.hpp>

#include <cstdio>
#include <string>


namespace boost { namespace memory {

/**
 * @brief A non-owning reference to a buffer.
 * @details
 * A buffer reference provides a range of utilities for describing a buffer - like
 * pointer to memory block where the buffer data resides and length of the data.
 * Additionally methods for accessing the data from the buffer are provided.
 */
class buffer_ref
{
public:
    /**
     * @brief Creates a reference through a pointer and length.
     * @param data A pointer to memory location.
     * @param length Number of bytes in the buffer.
     */
    template <typename pointer_type>
    buffer_ref(pointer_type* data, size_t length);

    /**
     * @brief Creates a reference through const pointer and length.
     * @param data A pointer to memory block.
     * @param length Length of the block.
     */
    template <typename pointer_type>
    buffer_ref(const pointer_type* data, size_t length);

    /**
     * @brief Creates a buffer reference from a type pointer.
     * @details
     * Creates buffer holds a pointer given in the constructor and length of the
     * type passed in the constructor.
     * @param pointer A pointer to a type.
     */
    template <typename pointer_type>
    buffer_ref(pointer_type* pointer);

    /**
     * @brief Creates a buffer reference from a memory block definition.
     * @param reference A **memory_block** the buffer is pointing at.
     */
    buffer_ref(const memory_block& block);

    /**
     * @brief Creates a buffer reference for a string.
     * @param s A string to get the buffer definition from.
     */
    buffer_ref(const std::string& s);

    /**
     * @brief Obtains a reference to the underlying memory block treating it as
     * the given **reference_type**.
     * @return A reference to the underlying type instance.
     */
    template <typename reference_type>
    reference_type as();

    /**
     * @brief Obtains a reference to the underlying memory block treating it as a
     * representation of the given **pointer_type**.
     * @return A pointer to the underlying entity.
     */
    template <typename pointer_type>
    pointer_type as_pointer();

    /**
     * @brief Number of bytes in the buffer.
     */
    size_t length() const;

    /**
     * @brief Obtains a sub buffer with the given size.
     * @param length The size of sub buffer.
     * @return A sub buffer definition.
     */
    buffer_ref subbuf(size_t length) const;
    /**
     * @brief Obtains a sub buffer with the given size.
     * @param offset Offset from the beginning of the buffer at which sub buffer should begin.
     * @param length The size of sub buffer.
     * @return A sub buffer definition.
     */
    buffer_ref subbuf(size_t offset, size_t length) const;

private:
    void* data_;
    size_t length_;
};

} }

#include "buffer_ref.ipp"

