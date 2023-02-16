#include "core/memory.h"
#include "core/memory_logging.h"
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
            std::size_t size = count * value_size;

            void * p = malloc(size);

            gaos::memory::log_malloc(p, size);

            return (value_type*)p;
        }

        void deallocate(value_type * p, std::size_t count) noexcept
        {
            std::size_t size = count * value_size;

            gaos::memory::log_free(p, size);

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