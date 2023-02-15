#include <array>
#include <cstring>
#include <iostream>


constexpr std::size_t ptr_byte_count = sizeof(void*);
using ptr_buffer = std::array<char, ptr_byte_count*2 + 3>;


char int_hex(std::size_t v)
{
    std::uint8_t clean_v = std::uint8_t(v & 0xF);
    return (clean_v < 10) ? '0' + clean_v : 'A' + (clean_v-10);
}


void fill_buffer_from_ptr(ptr_buffer &out_buffer, void const * ptr)
{
    char *write = &out_buffer.front();

    *(write++) = '0';
    *(write++) = 'x';
    
    std::size_t ptr_int;
    std::memcpy(&ptr_int, &ptr, sizeof(std::size_t));

    for (std::size_t i = 0; i < ptr_byte_count; ++i)
    {
        *(write++) = int_hex(ptr_int >> ((8 * (ptr_byte_count - 1 - i)) + 0));
        *(write++) = int_hex(ptr_int >> ((8 * (ptr_byte_count - 1 - i)) + 4));
    }

    *(write++) = 0;
}


void * operator new(std::size_t size)
{
    void * p = malloc(size);

    ptr_buffer buffer;
    fill_buffer_from_ptr(buffer, p);
    std::cout << "* malloc " << &buffer.front() << " " << size << std::endl;

    return p;
}


void operator delete(void * p)
{
    ptr_buffer buffer;
    fill_buffer_from_ptr(buffer, p);
    std::cout << "*   free " << &buffer.front() << std::endl;

    free(p);
}