#include <boost/memory.hpp>
#include <boost/memory/operators.hpp>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using namespace std;
using namespace boost::memory;

using my_allocator
        = fallback_allocator<
                free_list<
                        stack_allocator<
                                2048,
                                WORD_ALIGNMENT>,
                        4,
                        0,
                        32,
                        2048 / 32>,
                mallocator>;
DEFINE_ALLOCATOR_WITH_TRACE(my_allocator);

int main(int argc, char** argv)
{
    INIT_ALLOCATOR();

    vector<int> some_vector;
    some_vector.push_back(1);
    some_vector.push_back(1);

    return 0;
}
