#include "core/memory.h"

#include <array>
#include <cstring>
#include <iostream>


void * operator new(std::size_t size)
{
    namespace gm = gaos::memory;

    void * p = malloc(size);

    gm::ptr_buffer buffer;
    gm::fill_buffer_from_ptr(buffer, p);
    std::cout << "#    new " << &buffer.front() << " " << size << std::endl;

    return p;
}


void operator delete(void * p)
{
    namespace gm = gaos::memory;

    gm::ptr_buffer buffer;
    gm::fill_buffer_from_ptr(buffer, p);
    std::cout << "# delete " << &buffer.front() << std::endl;

    free(p);
}