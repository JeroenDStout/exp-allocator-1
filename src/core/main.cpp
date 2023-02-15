#include "version/git_version.h"

#include <iostream>


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
      << "Hello doctor"
      << std::endl;

    std::cout << "Hello Doctor" << std::endl;
    return 0;
}