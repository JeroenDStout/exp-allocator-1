#include <iostream>
#include <vector>
#include <unordered_map>


namespace gaos::tests {


    // Simply fill a vector step-by-step
    template<typename allocator_t>
    inline void test_vector(allocator_t& allocator)
    {
        if (gaos::memory::enable_logging) {
            std::cout
              << std::endl
              << "std::vector"
              << std::endl;
        }
          
        std::vector<int, allocator_t> test(allocator);

        gaos::memory::log_flush(true);
        
        if (gaos::memory::enable_logging) {
            std::cout
              << std::endl
              << "fill std::vector with 50 elements"
              << std::endl;
        }

        for (int i = 0; i < 500; ++i)
          test.push_back(i);

        gaos::memory::log_flush(true);
        
        if (gaos::memory::enable_logging) {
            std::cout
              << std::endl
              << "done"
              << std::endl;
        }
    }

    
    // Over multiple loops, fill an unordered_map with values,
    // then remove multiples of N, add multiples of N+1, etc
    // Every step collect them in a sub-step -- this represents
    // the generally messed up stuff that goes on with maps
    template<typename allocator_t>
    inline void test_map(allocator_t& allocator)
    {
        if (gaos::memory::enable_logging) {
            std::cout
              << std::endl
              << "std::unordered_map<int, int>"
              << std::endl;
        }

        std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, allocator_t> test(allocator);

        gaos::memory::log_flush(true);

        for (std::size_t step_size = 1; step_size <= 10; ++step_size) {
            if (step_size % 2 == 1) {
                if (gaos::memory::enable_logging) {
                    std::cout
                      << std::endl
                      << "increment multiples of " << step_size
                      << std::endl;
                }

                for (std::size_t i = 0; i < 1000; i += step_size)
                  test[(int)i] += (int)i;
            }
            else {
                if (gaos::memory::enable_logging) {
                    std::cout
                      << std::endl
                      << "remove multiples of " << step_size
                      << std::endl;
                }

                for (std::size_t i = 0; i < 1000; i += step_size)
                  test.erase((int)i);
            }

            gaos::memory::log_flush(true);

            {
                // This inner step is a prime candidate for a push-pop
                // as its allocations never leave this scope
                [[maybe_unused]] auto scope_pushpop = allocator.get_scoped_pushpop();

                std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, allocator_t> accumulate(allocator);
                
                if (gaos::memory::enable_logging) {
                    std::cout
                      << std::endl
                      << "accumulate frequencies"
                      << std::endl;
                }

                for (const auto elem : test)
                  accumulate[elem.second] += 1;

                accumulate.clear();
            }

            gaos::memory::log_flush(true);
        }
        
        if (gaos::memory::enable_logging) {
            std::cout
              << std::endl
              << "done"
              << std::endl;
        }
    }

}