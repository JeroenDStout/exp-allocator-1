#include "core/memory.h"
#include "core/memory_logging.h"

#include <iostream>
#include <limits>


namespace gaos::allocators {


    template <class T, class buffer_t>
    class ptr
    {
    public:
        using value_type = T;
        static constexpr std::size_t value_size = sizeof(value_type);

        buffer_t * internal_buffer;

        ptr(buffer_t * buffer) noexcept:
          internal_buffer(buffer)
        {}

        template <class U> ptr(ptr<U, buffer_t> const &rh) noexcept:
          internal_buffer(rh.internal_buffer)
        {}

        value_type * allocate(std::size_t count)
        {
            namespace gm = gaos::memory;

            std::size_t size = count * value_size;
            void * p = internal_buffer->allocate(size);

            return (value_type*)p;
        }

        void deallocate(value_type * p, std::size_t count) noexcept
        {
            namespace gm = gaos::memory;
            
            std::size_t size = count * value_size;

            internal_buffer->deallocate(p, size);
        }


        auto get_scoped_pushpop()
        {
            return internal_buffer->get_scoped_pushpop();
        }
    };


    template <class T, class U, class buffer_t>
    bool operator==(ptr<T, buffer_t> const& lh, ptr<U, buffer_t> const& rh) noexcept
    {
        return lh.internal_buffer == rh.internal_buffer;
    }


    template <class T, class U, class buffer_t>
    bool operator!=(ptr<T, buffer_t> const& lh, ptr<U, buffer_t> const& rh) noexcept
    {
        return !(lh == rh);
    }

}