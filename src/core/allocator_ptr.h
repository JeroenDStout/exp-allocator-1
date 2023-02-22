#include "core/memory_logging.h"

#include <iostream>
#include <limits>


namespace gaos::allocators {


    // Use an allocator referenced by pointer to do the internal
    // allocations: this is needed to have multiple containers all
    // use the same memory
    // Note that this expects to get an allocator which allocates bytes
    template <class T, class internal_allocator_t>
    class ptr
    {
    public:
      // -- Types

        using this_type  = ptr<T, internal_allocator_t>;
        using value_type = T;
        static constexpr std::size_t value_size = sizeof(value_type);
        
      // -- Members

        // The internal allocator that does all the actual allocations
        internal_allocator_t *internal_allocator;

      // -- Construction

        ptr(internal_allocator_t *allocator) noexcept
        : internal_allocator(allocator) {}

        template <class U> ptr(ptr<U, internal_allocator_t> const &rh) noexcept
        : internal_allocator(rh.internal_allocator) {}

      // -- Allocation

        auto allocate(std::size_t count) noexcept -> value_type * {
            // Allocate memory for (count × value_type)
            std::size_t size = count * value_size;

            // We just pass through to the internal allocator
            // Note we do not log, as we literally do not do
            // any contributions to the allocation
            void * p = internal_allocator->allocate(size);
            return (value_type*)p;
        }


        void deallocate(value_type * p, std::size_t count) noexcept {
            // Allocate memory for (count × value_type)
            std::size_t size = count * value_size;
            
            // We just pass through to the internal allocator
            // Note we do not log, as we literally do not do
            // any contributions to the deallocation
            internal_allocator->deallocate(p, size);
        }

        
        // Some allocators in this project can be scoped;
        // We pass this scoped pushpop request through too
        auto get_scoped_pushpop() noexcept {
            return internal_allocator->get_scoped_pushpop();
        }
    };

  // -- Operators

    template <class T, class U, class internal_allocator_t>
    bool operator==(ptr<T, internal_allocator_t> const& lh, ptr<U, internal_allocator_t> const& rh) noexcept {
        return lh.internal_allocator == rh.internal_allocator;
    }


    template <class T, class U, class internal_allocator_t>
    bool operator!=(ptr<T, internal_allocator_t> const& lh, ptr<U, internal_allocator_t> const& rh) noexcept {
        return !(lh == rh);
    }

}