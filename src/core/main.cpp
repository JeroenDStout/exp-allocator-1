#include "core/allocator_passthrough.h"
#include "core/tests.h"
#include "version/git_version.h"

#include <iostream>
#include <unordered_map>
#include <vector>


int main()
{
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

    gaos::allocators::passthrough<int> default_alloc_int;
    gaos::tests::test_vector(default_alloc_int);
    
    gaos::allocators::passthrough<std::pair<const int, int>> default_alloc_pair_int_int;
    gaos::tests::test_map(default_alloc_pair_int_int);

    return 0;
}