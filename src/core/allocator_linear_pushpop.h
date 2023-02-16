#include "core/memory.h"
#include "core/memory_logging.h"

#include <iostream>
#include <limits>


namespace gaos::allocators {


    template<std::size_t minimum_allocation_size, typename allocator_t = std::allocator<std::byte>>
    class linear_pushpop
    {
      public:
        using linear_pushpop_buffer_t = linear_pushpop<minimum_allocation_size, allocator_t>;

        struct blob_meta {
            std::uint32_t size;
            blob_meta*    next;
            blob_meta*    previous;
        };

        struct stack_data {
            blob_meta*    blob;
            std::uint32_t offset;
        } current_stack_data;

        static constexpr std::uint32_t blob_meta_size = sizeof(blob_meta);

        allocator_t internal_allocator;

        linear_pushpop(allocator_t allocator = {}) noexcept:
          internal_allocator(allocator)
        {
            current_stack_data.blob   = alloc_buffer(nullptr, minimum_allocation_size);
            current_stack_data.offset = blob_meta_size;
        }

        ~linear_pushpop() noexcept
        {
            clear();
            
            internal_allocator.deallocate((std::byte*)current_stack_data.blob, current_stack_data.blob->size);
        }


        void clear() noexcept
        {
            blob_meta *remove_next = current_stack_data.blob->next;

            for (;;) {
                if (remove_next == nullptr)
                  break;

                blob_meta *remove_current = remove_next;
                remove_next = remove_current->next;

                internal_allocator.deallocate((std::byte*)remove_current, remove_current->size);
            }
            
            remove_next = current_stack_data.blob;

            for (;;) {
                if (remove_next->previous == nullptr)
                  break;

                blob_meta *remove_current = remove_next;
                remove_next = remove_current->previous;

                internal_allocator.deallocate((std::byte*)remove_current, remove_current->size);
            }

            current_stack_data.blob       = remove_next;
            current_stack_data.blob->next = nullptr;
            current_stack_data.offset     = blob_meta_size;
        }

        void * allocate(std::size_t alloc_size_)
        {
            if (alloc_size_ > std::numeric_limits<std::uint32_t>::max())
              return nullptr;

            std::byte *ptr;
            std::uint32_t alloc_size = (std::uint32_t)alloc_size_;

            for (;;) {
                if (current_stack_data.offset + alloc_size < current_stack_data.blob->size) {
                    ptr = (std::byte*)(current_stack_data.blob) + current_stack_data.offset;
                    current_stack_data.offset += alloc_size;
                    break;
                }

                if (alloc_size + blob_meta_size > minimum_allocation_size) {
                    blob_meta *insert_blob = alloc_buffer(current_stack_data.blob->previous, alloc_size + blob_meta_size);
                    current_stack_data.blob->previous = insert_blob;
                    insert_blob->next = current_stack_data.blob;
                    ptr = (std::byte*)(insert_blob) + blob_meta_size;
                    break;
                }
                
                if (current_stack_data.blob->next != nullptr) {
                    current_stack_data.blob   = current_stack_data.blob->next;
                    current_stack_data.offset = blob_meta_size;
                    continue;
                }
                
                current_stack_data.blob   = alloc_buffer(current_stack_data.blob, minimum_allocation_size);
                current_stack_data.offset = blob_meta_size;
            }

            gaos::memory::log_allocate(ptr, alloc_size);
            return ptr;
        }


        void deallocate(void * ptr, std::size_t alloc_size)
        {
            gaos::memory::log_deallocate(ptr, alloc_size);
        }


        blob_meta * alloc_buffer(blob_meta *previous, std::uint32_t size)
        {
            std::byte *ptr  = internal_allocator.allocate(size);
            blob_meta *blob = (blob_meta*)ptr;

            if (previous != nullptr)
              previous->next = blob;

            blob->previous = previous;
            blob->size     = size;
            blob->next     = nullptr;

            return blob;
        }


        struct scoped_pushpop
        {
            linear_pushpop_buffer_t *buffer;
            stack_data              stack;

            scoped_pushpop(linear_pushpop_buffer_t* buffer):
              buffer(buffer),
              stack(buffer->current_stack_data)
            {
            }

            ~scoped_pushpop()
            {
                buffer->current_stack_data = stack;
            }
        };


        scoped_pushpop get_scoped_pushpop()
        {
            return scoped_pushpop(this);
        }
    };

}