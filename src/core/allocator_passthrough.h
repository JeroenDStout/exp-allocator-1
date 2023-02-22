#include "core/memory.h"
#include "core/memory_logging.h"

#include <iostream>
#include <limits>


namespace gaos::allocators {


    // Use an internal allocator for allocation -- all the credit, none of the work!
    // Useful template for creating smarter allocators, as backing
    // one allocator by another allows for a lot of utility
    // Note that this expects to get an allocator which allocates bytes
    template<typename allocator_t = std::allocator<std::byte>>
    class passthrough
    {
      public:
        // We use an internal allocator for all our allocations
        allocator_t internal_allocator;

      // -- Construction

        passthrough(allocator_t allocator = {}) noexcept
        : internal_allocator(allocator) {
        }

        ~passthrough() noexcept {
            clear();
        }

      // -- Allocation

        void clear() noexcept {
            // If we did internal memory management,
            // this is where would clean it up
        }

        auto allocate(std::size_t alloc_size) noexcept -> std::byte * {
            // Let the internal allocator do the work
            std::byte *ptr = internal_allocator.allocate(alloc_size);
            gaos::memory::log_allocate(ptr, alloc_size);
            return ptr;
        }


        void deallocate(void * ptr, std::size_t alloc_size) noexcept {
            // Let the internal allocator do the work
            gaos::memory::log_deallocate(ptr, alloc_size);
            internal_allocator.deallocate(ptr, alloc_size);
        }
        

        // Some allocators in this project can be scoped and
        // will return something sensible; this allocator does
        // not, and so just returns a dummy int
        auto get_scoped_pushpop() noexcept -> int {
            return 0;
        }
    };

}