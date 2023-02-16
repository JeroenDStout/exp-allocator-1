#include "core/allocator_linear_pushpop.h"
#include "core/allocator_passthrough.h"
#include "core/allocator_ptr.h"
#include "core/allocator_reuse.h"
#include "core/allocator_stack.h"
#include "core/tests.h"
#include "version/git_version.h"

#include <iostream>
#include <unordered_map>
#include <vector>


int main()
{
    namespace alloc   = gaos::allocators;
    namespace version = gaos::version;

    std::cout
      << std::endl
      << "          * * * * * * * * * * * * * * * * * " << std::endl
      << "         * * * Allocator Experiment I  * * * " << std::endl
      << "          * * * * * * * * * * * * * * * * * " << std::endl
      << std::endl
      << version::get_git_essential_version() << std::endl
      << version::get_compile_stamp() << std::endl
      << std::endl
      << version::get_git_history() << std::endl
      << std::endl;

    std::cout
      << "## " << std::endl
      << "## passthrough" << std::endl
      << "##" << std::endl;
    {
        gaos::memory::reset_meta_stats();

        alloc::passthrough<int> alloc_int;
        gaos::tests::test_vector(alloc_int);

        gaos::memory::log_flush(true);
    
        alloc::passthrough<std::pair<const int, int>> alloc_pair_int_int;
        gaos::tests::test_map(alloc_pair_int_int);

        gaos::memory::log_flush(true);

        std::cout << std::endl << "---------" << std::endl;
        gaos::memory::log_meta_stats();

        std::cout << std::endl;
    }

    std::cout
      << "## " << std::endl
      << "## stack_buffer" << std::endl
      << "##" << std::endl;
    {
        gaos::memory::reset_meta_stats();

        using stack_buffer = gaos::allocators::stack_buffer<1024, gaos::allocators::passthrough<std::byte>>;

        stack_buffer stack_buf0, stack_buf1;

        alloc::stack<int, stack_buffer> alloc_int(&stack_buf0);
        gaos::tests::test_vector(alloc_int);

        gaos::memory::log_flush(true);
   
        alloc::stack<std::pair<const int, int>, stack_buffer> alloc_pair_int_int(&stack_buf1);
        gaos::tests::test_map(alloc_pair_int_int);

        gaos::memory::log_flush(true);
        
        std::cout << std::endl << "---------" << std::endl;
        gaos::memory::log_meta_stats();

        std::cout << std::endl;
    }

    std::cout
      << "## " << std::endl
      << "## reuse" << std::endl
      << "##" << std::endl;
    {
        {
            gaos::memory::reset_meta_stats();

            constexpr std::size_t node_size =
            #ifdef _MSC_VER
              24;
            #else
              sizeof(std::unordered_map<const int, int>::node_type);
            #endif
            
            using reuse = gaos::allocators::reuse<node_size, gaos::allocators::passthrough<std::byte>>;
            reuse reuse_allocator;

            alloc::ptr<int, reuse> alloc_int(&reuse_allocator);
            gaos::tests::test_vector(alloc_int);

            gaos::memory::log_flush(true);

            alloc::ptr<std::pair<const int, int>, reuse> alloc_pair_int_int(&reuse_allocator);
            gaos::tests::test_map(alloc_pair_int_int);
            
            gaos::memory::log_flush(true);
        }

        gaos::memory::log_flush(true);

        std::cout << std::endl << "---------" << std::endl;
        gaos::memory::log_meta_stats();

        std::cout << std::endl;
    }

    std::cout
      << "## " << std::endl
      << "## linear_pushpop" << std::endl
      << "##" << std::endl;
    {
        {
            gaos::memory::reset_meta_stats();

            using linear_pushpop = gaos::allocators::linear_pushpop<1 << 16, gaos::allocators::passthrough<std::byte>>;
            linear_pushpop pushpop;

            {
                auto scoped_pushpop = pushpop.get_scoped_pushpop();

                alloc::ptr<int, linear_pushpop> alloc_int(&pushpop);
                gaos::tests::test_vector(alloc_int);

                gaos::memory::log_flush(true);
            }
            
            gaos::memory::log_flush(true);

            {
                auto scoped_pushpop = pushpop.get_scoped_pushpop();

                alloc::ptr<std::pair<const int, int>, linear_pushpop> alloc_pair_int_int(&pushpop);
                gaos::tests::test_map(alloc_pair_int_int);
            }
            
            gaos::memory::log_flush(true);
        }

        gaos::memory::log_flush(true);

        std::cout << std::endl << "---------" << std::endl;
        gaos::memory::log_meta_stats();

        std::cout << std::endl;
    }

    return 0;
}