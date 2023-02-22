#include <iostream>


namespace gaos::allocators {


    // First fill a (linear) buffer on the stack, then when
    // space runs out, start using the internal allocator
    // Note this expects an allocator which allocates bytes,
    // and that this is not an allocator to be used directly
    // with std containers, as it has no size type
    template<std::size_t size_on_stack, typename allocator_t = std::allocator<std::byte>>
    class stack
    {
      public:
      // -- Members

        std::byte                            *next_allocation;
        allocator_t                           internal_allocator;
        std::array<std::byte, size_on_stack>  buffer;

      // -- Construction
        
        stack() noexcept
        : next_allocation(&buffer.front()) {}

      // -- Allocation

        auto allocate(std::size_t alloc_size) noexcept -> void * {
            std::byte *ptr;

            // If we can fit our allocation in the buffer, we simply
            // return the address and move the next allocation ptr
            // If we cannot, we use the internal allocator
            if (next_allocation + alloc_size < &buffer.back())
            {
                ptr = next_allocation;
                next_allocation += alloc_size;
            }
            else
            {
                ptr = internal_allocator.allocate(alloc_size);
            }

            gaos::memory::log_allocate(ptr, alloc_size);
            return ptr;
        }


        void deallocate(void * ptr, std::size_t alloc_size) {
            gaos::memory::log_deallocate(ptr, alloc_size);

            // If the ptr came from our buffer, we do nothing, as deallocation
            // is a noop; otherwise pass the ptr on to the internal allocator
            if (ptr >= &buffer.front() && ptr < &buffer.back())
              return;

            internal_allocator.deallocate((std::byte*)ptr, alloc_size);
        }


        // Some allocators in this project can be scoped and
        // will return something sensible; this allocator does
        // not, and so just returns a dummy int
        auto get_scoped_pushpop() noexcept -> int {
            return 0;
        }
    };

}