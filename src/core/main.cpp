#include "core/allocator_passthrough.h"
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
      << std::endl
      << "Hello doctor" << std::endl;

    {
        alloc::passthrough<int> alloc_int;
        gaos::tests::test_vector(alloc_int);
    
        alloc::passthrough<std::pair<const int, int>> alloc_pair_int_int;
        gaos::tests::test_map(alloc_pair_int_int);
    }
    {
        using stack_buffer = gaos::allocators::stack_buffer<1024>;

        stack_buffer stack_buf0, stack_buf1;

        alloc::stack<int, stack_buffer> alloc_int(&stack_buf0);
        gaos::tests::test_vector(alloc_int);
   
        alloc::stack<std::pair<const int, int>, stack_buffer> alloc_pair_int_int(&stack_buf1);
        gaos::tests::test_map(alloc_pair_int_int);
    }

    return 0;
}