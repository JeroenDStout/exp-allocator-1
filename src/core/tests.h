#include <iostream>
#include <vector>
#include <unordered_map>


namespace gaos::tests {

    template<typename allocator_t>
    inline void test_vector(allocator_t& allocator)
    {
        std::cout
          << std::endl
          << "std::vector"
          << std::endl;
          
        std::vector<int, allocator_t> test(allocator);

        gaos::memory::log_flush(true);

        std::cout
          << std::endl
          << "fill std::vector with 50 elements"
          << std::endl;

        for (int i = 0; i < 50; ++i)
          test.push_back(i);

        gaos::memory::log_flush(true);

        std::cout
          << std::endl
          << "done"
          << std::endl;
    }

    template<typename allocator_t>
    inline void test_map(allocator_t& allocator)
    {
        std::cout
          << std::endl
          << "std::unordered_map<int, int> (2x)"
          << std::endl;

        std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, allocator_t> test(allocator);

        gaos::memory::log_flush(true);

        for (std::size_t step_size = 1; step_size <= 10; ++step_size)
        {
            if (step_size % 2 == 1)
            {
                std::cout
                  << std::endl
                  << "increment multiples of " << step_size
                  << std::endl;

                for (std::size_t i = 0; i < 1000; i += step_size)
                  test[(int)i] += (int)i;
            }
            else
            {
                std::cout
                  << std::endl
                  << "remove multiples of " << step_size
                  << std::endl;

                for (std::size_t i = 0; i < 1000; i += step_size)
                  test.erase((int)i);
            }

            gaos::memory::log_flush(true);

            {
                [[maybe_unused]] auto scope_pushpop = allocator.get_scoped_pushpop();

                std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, allocator_t> accumulate(allocator);

                std::cout
                  << std::endl
                  << "accumulate frequencies"
                  << std::endl;

                for (const auto elem : test)
                  accumulate[elem.second] += 1;

                accumulate.clear();
            }

            gaos::memory::log_flush(true);
        }

        std::cout
          << std::endl
          << "done"
          << std::endl;
    }

}