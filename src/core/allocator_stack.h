#include "core/memory.h"
#include <iostream>


namespace gaos::allocators {


    template<std::size_t size_on_stack, typename allocator_t = std::allocator<std::byte>>
    class stack_buffer
    {
      public:
        std::byte*                           next_allocation;
        allocator_t                          internal_allocator;
        std::array<std::byte, size_on_stack> buffer;
        
        stack_buffer() noexcept:
          next_allocation(&buffer.front())
        {}

        void * allocate(std::size_t alloc_size)
        {
            if (next_allocation + alloc_size < &buffer.back())
            {
                std::byte * ret = next_allocation;
                next_allocation += alloc_size;
                return ret;
            }

            return internal_allocator.allocate(alloc_size);
        }


        void deallocate(void * ptr, std::size_t alloc_size)
        {
            if (ptr >= &buffer.front() && ptr < &buffer.back())
              return;

            internal_allocator.deallocate((std::byte*)ptr, alloc_size);
        }
    };


    template <class T, class buffer_t>
    class stack
    {
    public:
        using value_type = T;
        static constexpr std::size_t value_size = sizeof(value_type);

        buffer_t * internal_buffer;

        stack(buffer_t * buffer) noexcept:
          internal_buffer(buffer)
        {}

        template <class U> stack(stack<U, buffer_t> const &rh) noexcept:
          internal_buffer(rh.internal_buffer)
        {}

        value_type * allocate(std::size_t count)
        {
            namespace gm = gaos::memory;

            std::size_t size = count * value_size;
            void * p = internal_buffer->allocate(size);
            
            gm::ptr_buffer buffer;
            gm::fill_buffer_from_ptr(buffer, p);
            std::cout
              << "~    --> " << &buffer.front() << " " << size
              << " (" << count << "x " << value_size << ") " << std::endl;

            return (value_type*)p;
        }

        void deallocate(value_type * p, std::size_t count) noexcept
        {
            namespace gm = gaos::memory;
            
            std::size_t size = count * value_size;

            gm::ptr_buffer buffer;
            gm::fill_buffer_from_ptr(buffer, p);
            std::cout
              << "~    <-- " << &buffer.front()
              << " (" << count << "x)" << std::endl;

            internal_buffer->deallocate(p, size);
        }
    };


    template <class T, class U, class buffer_t>
    bool operator==(stack<T, buffer_t> const&, stack<U, buffer_t> const&) noexcept
    {
        return true;
    }


    template <class T, class U, class buffer_t>
    bool operator!=(stack<T, buffer_t> const& lh, stack<U, buffer_t> const& rh) noexcept
    {
        return !(lh == rh);
    }

}