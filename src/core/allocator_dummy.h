#include "core/memory.h"
#include "core/memory_logging.h"

#include <iostream>
#include <limits>


namespace gaos::allocators {


    template<typename allocator_t = std::allocator<std::byte>>
    class dummy
    {
      public:
        allocator_t internal_allocator;

        dummy(allocator_t allocator = {}) noexcept:
          internal_allocator(allocator)
        {
        }

        ~dummy() noexcept
        {
            clear();
        }


        void clear() noexcept
        {
        }

        void * allocate(std::size_t alloc_size)
        {
            std::byte *ptr = internal_allocator.allocate(alloc_size);
            gaos::memory::log_allocate(ptr, alloc_size);
            return ptr;
        }


        void deallocate(void * ptr, std::size_t alloc_size)
        {
            gaos::memory::log_deallocate(ptr, alloc_size);
            internal_allocator.deallocate(ptr, alloc_size);
        }
    };

}