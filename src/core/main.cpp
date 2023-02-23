#include "core/allocator_libc.h"
#include "core/allocator_linear_pushpop.h"
#include "core/allocator_ptr.h"
#include "core/allocator_reuse.h"
#include "core/allocator_stack.h"
#include "core/tests.h"
#include "version/git_version.h"

#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>


void main_speed_test()
{
    gaos::memory::enable_logging = false;

    namespace alloc = gaos::allocators;

    using us    = std::chrono::microseconds;
    using clock = std::chrono::high_resolution_clock;

    int repeat_count = 1000;

    std::uint64_t sum_time_passthrough     = 0
                , sum_time_stack_buffer    = 0
                , sum_time_reuse           = 0
                , sum_time_linear_pushpop  = 0;
    std::uint64_t max_alloc_passthrough    = 0
                , max_alloc_stack_buffer   = 0
                , max_alloc_reuse          = 0
                , max_alloc_linear_pushpop = 0;
    std::uint64_t max_mem_passthrough      = 0
                , max_mem_stack_buffer     = 0
                , max_mem_reuse            = 0
                , max_mem_linear_pushpop   = 0;

    std::cout
      << "running vector experiment " << repeat_count << " times..." << std::endl << std::endl;

    {
        // passthrough

        sum_time_passthrough = 0;

        for (int i = 0; i < repeat_count; ++i) {
            gaos::memory::reset_meta_stats();
            
            alloc::libc<int> alloc_int;

            auto time_start = clock::now();
            gaos::tests::test_vector(alloc_int);
            auto time_end   = clock::now();

            sum_time_passthrough  += std::chrono::duration_cast<us>(time_end - time_start).count();
            max_alloc_passthrough  = gaos::memory::count_malloc;
            max_mem_passthrough    = gaos::memory::size_malloc_peak;
        }

        // stack

        sum_time_stack_buffer = 0;

        for (int i = 0; i < repeat_count; ++i) {
            gaos::memory::reset_meta_stats();
            
            using stack = gaos::allocators::stack<1 << 14, gaos::allocators::libc<std::byte>>;
            stack stack_buf0;
            alloc::ptr<int, stack> alloc_int(&stack_buf0);

            auto time_start = clock::now();
            gaos::tests::test_vector(alloc_int);
            auto time_end   = clock::now();

            sum_time_stack_buffer  += std::chrono::duration_cast<us>(time_end - time_start).count();
            max_alloc_stack_buffer  = gaos::memory::count_malloc;
            max_mem_stack_buffer    = gaos::memory::size_malloc_peak;
        }

        // reuse

        sum_time_reuse = 0;

        for (int i = 0; i < repeat_count; ++i) {
            gaos::memory::reset_meta_stats();
            
            constexpr std::size_t node_size =
            #ifdef _MSC_VER
              24;
            #else
              sizeof(std::unordered_map<const int, int>::node_type);
            #endif

            using reuse = gaos::allocators::reuse<node_size, gaos::allocators::libc<std::byte>>;
            reuse reuse_allocator;
            alloc::ptr<int, reuse> alloc_int(&reuse_allocator);

            auto time_start = clock::now();
            gaos::tests::test_vector(alloc_int);
            auto time_end   = clock::now();

            sum_time_reuse  += std::chrono::duration_cast<us>(time_end - time_start).count();
            max_alloc_reuse  = gaos::memory::count_malloc;
            max_mem_reuse    = gaos::memory::size_malloc_peak;
        }

        // linear_pushpop

        sum_time_linear_pushpop = 0;

        for (int i = 0; i < repeat_count; ++i) {
            gaos::memory::reset_meta_stats();
            
            using linear_pushpop = gaos::allocators::linear_pushpop<1 << 14, gaos::allocators::libc<std::byte>>;

            linear_pushpop pushpop;
            alloc::ptr<int, linear_pushpop> alloc_int(&pushpop);

            auto time_start = clock::now();
            gaos::tests::test_vector(alloc_int);
            auto time_end   = clock::now();

            sum_time_linear_pushpop  += std::chrono::duration_cast<us>(time_end - time_start).count();
            max_alloc_linear_pushpop  = gaos::memory::count_malloc;
            max_mem_linear_pushpop    = gaos::memory::size_malloc_peak;
        }
    }
    
    std::cout
      << "passthrough    "
        <<                 std::setw(6) << (sum_time_passthrough / repeat_count) << "us"
        << " | malloc " << std::setw(6) << max_alloc_passthrough << "x"
        << " | peak   " << std::setw(6) << max_mem_passthrough << "B"
        << std::endl
      << "stack_buffer   "
        <<                 std::setw(6) << (sum_time_stack_buffer / repeat_count) << "us"
        << " | malloc " << std::setw(6) << max_alloc_stack_buffer << "x"
        << " | peak   " << std::setw(6) << max_mem_stack_buffer << "B"
        << std::endl
      << "reuse          "
        <<                 std::setw(6) << (sum_time_reuse / repeat_count) << "us"
        << " | malloc " << std::setw(6) << max_alloc_reuse << "x"
        << " | peak   " << std::setw(6) << max_mem_reuse << "B"
        << std::endl
      << "linear_pushpop "
        <<                 std::setw(6) << (sum_time_linear_pushpop / repeat_count) << "us"
        << " | malloc " << std::setw(6) << max_alloc_linear_pushpop << "x"
        << " | peak   " << std::setw(6) << max_mem_linear_pushpop << "B"
        << std::endl
        << std::endl;

    std::cout
      << "running map experiment " << repeat_count << " times..." << std::endl << std::endl;

    {
        // passthrough

        sum_time_passthrough = 0;

        for (int i = 0; i < repeat_count; ++i) {
            gaos::memory::reset_meta_stats();
            
            alloc::libc<std::pair<const int, int>> alloc_pair_int_int;

            auto time_start = clock::now();
            gaos::tests::test_map(alloc_pair_int_int);
            auto time_end   = clock::now();

            sum_time_passthrough  += std::chrono::duration_cast<us>(time_end - time_start).count();
            max_alloc_passthrough  = gaos::memory::count_malloc;
            max_mem_passthrough    = gaos::memory::size_malloc_peak;
        }

        // stack

        sum_time_stack_buffer = 0;

        for (int i = 0; i < repeat_count; ++i) {
            gaos::memory::reset_meta_stats();
            
            using stack = gaos::allocators::stack<1 << 14, gaos::allocators::libc<std::byte>>;
            stack stack_buf0;
            alloc::ptr<std::pair<const int, int>, stack> alloc_pair_int_int(&stack_buf0);

            auto time_start = clock::now();
            gaos::tests::test_map(alloc_pair_int_int);
            auto time_end   = clock::now();

            sum_time_stack_buffer  += std::chrono::duration_cast<us>(time_end - time_start).count();
            max_alloc_stack_buffer  = gaos::memory::count_malloc;
            max_mem_stack_buffer    = gaos::memory::size_malloc_peak;
        }

        // reuse

        sum_time_reuse = 0;

        for (int i = 0; i < repeat_count; ++i) {
            gaos::memory::reset_meta_stats();
            
            constexpr std::size_t node_size =
            #ifdef _MSC_VER
              24;
            #else
              sizeof(std::unordered_map<const int, int>::node_type);
            #endif

            using reuse = gaos::allocators::reuse<node_size, gaos::allocators::libc<std::byte>>;
            reuse reuse_allocator;
            alloc::ptr<std::pair<const int, int>, reuse> alloc_pair_int_int(&reuse_allocator);

            auto time_start = clock::now();
            gaos::tests::test_map(alloc_pair_int_int);
            auto time_end   = clock::now();

            sum_time_reuse  += std::chrono::duration_cast<us>(time_end - time_start).count();
            max_alloc_reuse  = gaos::memory::count_malloc;
            max_mem_reuse    = gaos::memory::size_malloc_peak;
        }

        // linear_pushpop

        sum_time_linear_pushpop = 0;

        for (int i = 0; i < repeat_count; ++i) {
            gaos::memory::reset_meta_stats();
            
            using linear_pushpop = gaos::allocators::linear_pushpop<1 << 14, gaos::allocators::libc<std::byte>>;

            linear_pushpop pushpop;
            alloc::ptr<std::pair<const int, int>, linear_pushpop> alloc_pair_int_int(&pushpop);

            auto time_start = clock::now();
            gaos::tests::test_map(alloc_pair_int_int);
            auto time_end   = clock::now();

            sum_time_linear_pushpop  += std::chrono::duration_cast<us>(time_end - time_start).count();
            max_alloc_linear_pushpop  = gaos::memory::count_malloc;
            max_mem_linear_pushpop    = gaos::memory::size_malloc_peak;
        }
    }
    
    std::cout
      << "passthrough    "
        <<                 std::setw(6) << (sum_time_passthrough / repeat_count) << "us"
        << " | malloc " << std::setw(6) << max_alloc_passthrough << "x"
        << " | peak   " << std::setw(6) << max_mem_passthrough << "B"
        << std::endl
      << "stack_buffer   "
        <<                 std::setw(6) << (sum_time_stack_buffer / repeat_count) << "us"
        << " | malloc " << std::setw(6) << max_alloc_stack_buffer << "x"
        << " | peak   " << std::setw(6) << max_mem_stack_buffer << "B"
        << std::endl
      << "reuse          "
        <<                 std::setw(6) << (sum_time_reuse / repeat_count) << "us"
        << " | malloc " << std::setw(6) << max_alloc_reuse << "x"
        << " | peak   " << std::setw(6) << max_mem_reuse << "B"
        << std::endl
      << "linear_pushpop "
        <<                 std::setw(6) << (sum_time_linear_pushpop / repeat_count) << "us"
        << " | malloc " << std::setw(6) << max_alloc_linear_pushpop << "x"
        << " | peak   " << std::setw(6) << max_mem_linear_pushpop << "B"
        << std::endl;
}


void main_long_log()
{
    namespace alloc   = gaos::allocators;

    std::cout
      << "## " << std::endl
      << "## passthrough" << std::endl
      << "##" << std::endl;

    {
        gaos::memory::reset_meta_stats();

        alloc::libc<int> alloc_int;
        gaos::tests::test_vector(alloc_int);

        gaos::memory::log_flush(true);
    
        alloc::libc<std::pair<const int, int>> alloc_pair_int_int;
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

        using stack = gaos::allocators::stack<1024, gaos::allocators::libc<std::byte>>;

        stack stack_buf0, stack_buf1;

        alloc::ptr<int, stack> alloc_int(&stack_buf0);
        gaos::tests::test_vector(alloc_int);

        gaos::memory::log_flush(true);
   
        alloc::ptr<std::pair<const int, int>, stack> alloc_pair_int_int(&stack_buf1);
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
            
            using reuse = gaos::allocators::reuse<node_size, gaos::allocators::libc<std::byte>>;
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

            using linear_pushpop = gaos::allocators::linear_pushpop<1 << 16, gaos::allocators::libc<std::byte>>;
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
}


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
      << std::endl;

    main_speed_test();

    return 0;
}