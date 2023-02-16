#include "core/memory.h"
#include "core/memory_logging.h"

#include <iostream>
#include <limits>


namespace gaos::allocators {


    template<std::size_t max_alloc_size, typename allocator_t = std::allocator<std::byte>>
    class reuse
    {
      public:
        using reuse_t = reuse<max_alloc_size, allocator_t>;

        allocator_t internal_allocator;

        std::byte* next = nullptr;

        reuse() noexcept
        {}

        reuse(allocator_t allocator) noexcept:
          internal_allocator(allocator)
        {}

        ~reuse() noexcept
        {
            clear();
        }


        void clear() noexcept
        {
            while (next != nullptr)
            {
                std::byte* ptr = next;
                next = *(std::byte**)(next);
                internal_allocator.deallocate(ptr, max_alloc_size);
            }
        }

        void * allocate(std::size_t alloc_size)
        {
            std::byte *ptr;

            if (alloc_size <= max_alloc_size)
            {
                if (next != nullptr)
                {
                    ptr = next;
                    next = *(std::byte**)(next);
                }
                else
                {
                    ptr = (next != nullptr)? next : (std::byte*)internal_allocator.allocate(max_alloc_size);
                }
            }
            else
            {
                ptr = (std::byte*)internal_allocator.allocate(alloc_size);
            }

            gaos::memory::log_allocate(ptr, alloc_size);
            return ptr;
        }


        void deallocate(void * ptr, std::size_t alloc_size)
        {
            if (alloc_size <= max_alloc_size)
            {
                gaos::memory::log_deallocate(ptr, max_alloc_size);

                std::byte *old_next = next;
                next = (std::byte*)ptr;
                *(std::byte**)(next) = old_next;
            }
            else
            {
                gaos::memory::log_deallocate(ptr, alloc_size);
                internal_allocator.deallocate((std::byte*)ptr, alloc_size);
            }
        }


        auto get_scoped_pushpop()
        {
            return 1;
        }
    };

}