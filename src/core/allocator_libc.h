#include "core/memory.h"
#include "core/memory_logging.h"
#include <iostream>


namespace gaos::allocators {

    // Use the libc style malloc/free to manage memory
    // Quite analogous to the default std allocator, except
    // that this explicitly uses the c runtime instead of new/delete
    template <class T>
    class libc
    {
      public:
      // -- Types

        using value_type = T;
        static constexpr std::size_t value_size = sizeof(value_type);

      // -- Construction

        libc() noexcept {}
        template <class U> libc(libc<U> const&) noexcept {}

      // -- Allocation

        auto allocate(std::size_t count) noexcept -> value_type * {
            // Allocate memory for (count × value_type)
            std::size_t size = count * value_size;

            // We just pass through to malloc
            void *p = malloc(size);
            gaos::memory::log_malloc(p, size);

            return (value_type*)p;
        }

        void deallocate(value_type * p, std::size_t count) noexcept {
            // Free memory for (count × value_type)
            std::size_t size = count * value_size;
            
            // We just pass through to free
            gaos::memory::log_free(p, size);
            free(p);
        }


        // Some allocators in this project can be scoped and
        // will return something sensible; this allocator does
        // not, and so just returns a dummy int
        auto get_scoped_pushpop() noexcept -> int {
            return 0;
        }
    };

  // -- Operators

    template <class T, class U>
    bool operator==(libc<T> const&, libc<U> const&) noexcept {
        return true;
    }


    template <class T, class U>
    bool operator!=(libc<T> const& x, libc<U> const& y) noexcept {
        return !(x == y);
    }

}