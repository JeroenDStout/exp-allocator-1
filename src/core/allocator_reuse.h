#include "core/memory.h"
#include "core/memory_logging.h"

#include <iostream>
#include <limits>


namespace gaos::allocators {


    // Try to reuse memory of a fixed size by creating a linked list
    // When no memory is available, use an internal allocator
    // Note this expects an allocator which allocates bytes,
    // and that this is not an allocator to be used directly
    // with std containers, as it has no size type
    template<std::size_t fixed_alloc_size, typename allocator_t = std::allocator<std::byte>>
    class reuse
    {
      public:
      // -- Members

        allocator_t  internal_allocator;
        std::byte   *next = nullptr;
        
      // -- Construction

        reuse() noexcept {}
        reuse(allocator_t allocator) noexcept
        : internal_allocator(allocator) {}

        ~reuse() noexcept {
            clear();
        }
        
      // -- Allocation

        void clear() noexcept
        {
            // Remove all allocations by following the linked list
            // and deallocating one by one -- note that in a real
            // scenario, we might be backed by a linear allocator
            // so we might be doing a lot of unneeded ptr chasing!
            while (next != nullptr) {
                // Get the next ptr in the chain
                std::byte* ptr = next;
                next = *(std::byte**)(next);

                // Deallocate
                gaos::memory::log_deallocate(ptr, fixed_alloc_size);
                internal_allocator.deallocate(ptr, fixed_alloc_size);
            }
        }


        auto allocate(std::size_t alloc_size) noexcept -> void * {
            std::byte *ptr;

            // If the allocation size is leq to our fixed size we try to
            // reuse an old piece of memory -- otherwise, we immediately
            // pass on this allocation req to the internal allocator
            if (alloc_size <= fixed_alloc_size)
            {
                // If we have a next reusable allocation, we return it
                // and move our next ptr to the linked location
                // Otherwise, we use the internal allocator
                if (next != nullptr)
                {
                    ptr = next;
                    next = *(std::byte**)(next);
                }
                else
                {
                    ptr = (next != nullptr)? next : (std::byte*)internal_allocator.allocate(fixed_alloc_size);
                }
            }
            else
            {
                ptr = (std::byte*)internal_allocator.allocate(alloc_size);
            }

            gaos::memory::log_allocate(ptr, alloc_size);
            return ptr;
        }


        void deallocate(void * ptr, std::size_t alloc_size) noexcept {
            // If the allocation size is leq to our fixed size we reuse the
            // memory; otherwise we pass on to the internal allocator
            if (alloc_size <= fixed_alloc_size)
            {
                gaos::memory::log_deallocate(ptr, fixed_alloc_size);

                // Make our next ptr point to the now available memory,
                // and store the previous next ptr to create the chain
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

        
        // Some allocators in this project can be scoped;
        // We pass this scoped pushpop request through too
        // In fact, be careful when backing a reuse with a
        // scoped allocator -- it may want to reuse memory
        // that was popped by the scope!
        auto get_scoped_pushpop() noexcept -> int {
            return 1;
        }
    };

}