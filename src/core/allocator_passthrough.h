#include "core/memory.h"
#include <iostream>


namespace gaos::allocators {

    template <class T>
    class passthrough
    {
    public:
        using value_type = T;
        static constexpr std::size_t value_size = sizeof(value_type);

        passthrough() noexcept {}
        template <class U> passthrough(passthrough<U> const&) noexcept {}

        value_type * allocate(std::size_t count)
        {
            namespace gm = gaos::memory;

            std::size_t size = count * value_size;
            void * p = malloc(size);

            gm::ptr_buffer buffer;
            gm::fill_buffer_from_ptr(buffer, p);
            std::cout
              << "* malloc " << &buffer.front() << " " << size
              << " (" << count << "x " << value_size << ") " << std::endl;

            return (value_type*)p;
        }

        void deallocate(value_type * p, std::size_t count) noexcept
        {
            namespace gm = gaos::memory;

            gm::ptr_buffer buffer;
            gm::fill_buffer_from_ptr(buffer, p);
            std::cout
              << "*   free " << &buffer.front()
              << " (" << count << "x)" << std::endl;

            free(p);
        }
    };


    template <class T, class U>
    bool operator==(passthrough<T> const&, passthrough<U> const&) noexcept
    {
        return true;
    }


    template <class T, class U>
    bool operator!=(passthrough<T> const& x, passthrough<U> const& y) noexcept
    {
        return !(x == y);
    }

}