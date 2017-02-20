#include <boost/memory.hpp>
#include <boost/memory/operators.hpp>

#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
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
DEFINE_ALLOCATOR(my_allocator);

mutex m;

void thread_main()
{
    int first;
    INIT_ALLOCATOR();

    vector<char> v(16, 0);

    lock_guard<mutex> lock(m);
    cout << "thread-id: " << this_thread::get_id() << " "
        << "first:" << &first << " "
        << "data:" << reinterpret_cast<void*>(v.data()) << endl;
}


int main(int argc, char** argv) 
{
    thread t1(thread_main);
    thread t2(thread_main);
    
    t1.join();
    t2.join();
    
    return 0;
}

