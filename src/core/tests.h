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

        std::cout
          << std::endl
          << "fill std::vector with 50 elements"
          << std::endl;

        for (int i = 0; i < 50; ++i)
          test.push_back(i);

        std::cout
          << std::endl
          << " # # # "
          << std::endl;
    }

    template<typename allocator_t>
    inline void test_map(allocator_t& allocator)
    {
        std::cout
          << std::endl
          << "std::unordered_map<int, int>"
          << std::endl;

        std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, allocator_t> test(allocator);

        std::cout
          << std::endl
          << "fill with 50 elements"
          << std::endl;

        for (int i = 0; i < 50; ++i)
          test[i] = i;

        std::cout
          << std::endl
          << "remove 25 elements"
          << std::endl;

        for (int i = 0; i < 25; ++i)
          test.erase(i*2);

        std::cout
          << std::endl
          << "fill with original 50 elements"
          << std::endl;

        for (int i = 0; i < 50; ++i)
          test[i] = i;

        std::cout
          << std::endl
          << " # # # "
          << std::endl;
    }

}