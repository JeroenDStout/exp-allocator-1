#include "core/memory_logging.h"

#include <iostream>
#include <limits>


namespace gaos::allocators {


    // Grab blobs of memory and linearly allocate within them,
    // with deallocation being a noop -- to avoid heavy memory
    // filling, has an internal stack which can be popped, freeing
    // up all memory that was claimed within a scope
    // Note this expects an allocator which allocates bytes,
    // and that this is not an allocator to be used directly
    // with std containers, as it has no size type
    template<std::size_t min_blob_size, typename allocator_t = std::allocator<std::byte>>
    class linear_pushpop
    {
      public:
      // -- Types

        using this_t = linear_pushpop<min_blob_size, allocator_t>;

        // Information as a header in the blob, effectively
        // making the blobs a doubly linked list
        struct blob_meta {
            std::uint32_t size;
            blob_meta*    next;
            blob_meta*    previous;
        };
        static constexpr std::uint32_t blob_meta_size = sizeof(blob_meta);

        // Stack data referencing a specific blob and the offset
        // in it to the next free allocation
        struct stack_data {
            blob_meta*    blob;
            std::uint32_t offset;
        };

      // -- Members

        stack_data  current_stack_data;
        allocator_t internal_allocator;

      // -- Construction

        linear_pushpop(allocator_t allocator = {}) noexcept
        : internal_allocator(allocator) {
            // Upon construction, immediately grab a blob
            current_stack_data.blob   = alloc_buffer(nullptr, min_blob_size);
            current_stack_data.offset = blob_meta_size;
        }


        ~linear_pushpop() noexcept {
            clear();
            
            // Deallocate the initial blob
            internal_allocator.deallocate((std::byte*)current_stack_data.blob, current_stack_data.blob->size);
        }

      // -- Allocation
      
        // Remove all the blobs except the first in the chain,
        // deallocating with the internal allocator
        void clear() noexcept {
            blob_meta *remove_next;

            // For every blob following the stack's current blob, remove it
            for (remove_next = current_stack_data.blob->next; remove_next != nullptr;) {
                blob_meta *remove_current = remove_next;
                remove_next = remove_current->next;

                internal_allocator.deallocate((std::byte*)remove_current, remove_current->size);
            }

            // For every blob before and including the stack's current blob, remove it
            for (remove_next = current_stack_data.blob; remove_next->previous != nullptr;) {
                blob_meta *remove_current = remove_next;
                remove_next = remove_current->previous;

                internal_allocator.deallocate((std::byte*)remove_current, remove_current->size);
            }

            // Set our stack to the first blob with an empty blob offset
            current_stack_data.blob       = remove_next;
            current_stack_data.blob->next = nullptr;
            current_stack_data.offset     = blob_meta_size;
        }


        auto allocate(std::size_t alloc_size_) -> void * {
            // We use 32 bits to store the allocation size in a blob,
            // so prevent larger blobs from existing -- any maniac
            // allocating over 4GB with a linear allocator deserves
            // the nullptr coming to them
            if (alloc_size_ > std::numeric_limits<std::uint32_t>::max())
              return nullptr;

            std::byte *ptr;
            std::uint32_t alloc_size = (std::uint32_t)alloc_size_;

            // Over multiple steps, try to allocate
            for (;;) {
                // If the allocation fits within our current blob, grab it
                if (current_stack_data.offset + alloc_size <= current_stack_data.blob->size) {
                    ptr = (std::byte*)(current_stack_data.blob) + current_stack_data.offset;
                    current_stack_data.offset += alloc_size;
                    break;
                }

                // If the current blob is empty and our allocation is too large for it,
                // insert a blob *before* the current blob that perfectly fits it
                // We insert it before so that if we pop, the larger buffer is left earlier
                // within the list, and if we do similar large allocations in a row, it will
                // be reused more frequently [citation needed]
                if (current_stack_data.offset == blob_meta_size && alloc_size + blob_meta_size > min_blob_size) {
                    blob_meta *insert_blob = alloc_buffer(current_stack_data.blob->previous, alloc_size + blob_meta_size);
                    current_stack_data.blob->previous = insert_blob;
                    insert_blob->next = current_stack_data.blob;
                    ptr = (std::byte*)(insert_blob) + blob_meta_size;
                    break;
                }
                
                // If there is another blob, try using it
                if (current_stack_data.blob->next != nullptr) {
                    current_stack_data.blob   = current_stack_data.blob->next;
                    current_stack_data.offset = blob_meta_size;
                    continue;
                }
                
                // If we ran out of blobs, allocate a new one and restart this cycle
                current_stack_data.blob   = alloc_buffer(current_stack_data.blob, min_blob_size);
                current_stack_data.offset = blob_meta_size;
            }

            gaos::memory::log_allocate(ptr, alloc_size);
            return ptr;
        }


        void deallocate(void *ptr, std::size_t alloc_size) {
            // Deallocation is a noop -- this makes this allocator fast
            // but obviously with many repeated allocations it wastes enormous
            // amounts of space
            gaos::memory::log_deallocate(ptr, alloc_size);
        }

      protected:
        auto alloc_buffer(blob_meta *previous, std::uint32_t size) -> blob_meta * {
            // Use the internal allocator to grab a new blob
            std::byte *ptr  = internal_allocator.allocate(size);
            blob_meta *blob = (blob_meta*)ptr;

            // Link it to the existing blobs
            if (previous != nullptr)
              previous->next = blob;
            blob->previous = previous;
            blob->size     = size;
            blob->next     = nullptr;

            return blob;
        }


      public:
        // Struct to store a copy of the stack and replace it, thereby
        // effectively popping all allocations after it was constructed
        struct scoped_pushpop {
            this_t     *buffer;
            stack_data  stack;

            scoped_pushpop(this_t* buffer):
              buffer(buffer), stack(buffer->current_stack_data) {}

            ~scoped_pushpop() {
                buffer->current_stack_data = stack;
            }
        };


        // Create a scoped pushpop struct pointing to ourselves
        auto get_scoped_pushpop() -> scoped_pushpop {
            return scoped_pushpop(this);
        }
    };

}