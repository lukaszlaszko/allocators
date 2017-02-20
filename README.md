[![Sources](https://img.shields.io/badge/bitbucket-sources-green.svg?style=flat)](https://bitbucket.org/lukaszlaszko/allocators/)
[![Pipelines](https://img.shields.io/badge/bitbucket-pipelines-blue.svg?style=flat)](https://bitbucket.org/lukaszlaszko/allocators/addon/pipelines/home#!/)
[![Documentation](https://img.shields.io/badge/bitbucket-documentation-orange.svg?
style=flat)](http://lukaszlaszko.bitbucket.io/allocators.git/master)
## Motivation ##

This repository provides implementation of composable allocators described by [Andrei Alexandrescu](http://erdani.com) on [CppCon 2015](https://www.youtube.com/watch?v=LIb3L4vKZ7U)

Following allocators have been implemented:

* **null_allocator**
* **mallocator**
* **fallback_allocator**
* **stack_allocator**
* **free_list**
* **segregator**
* **bucketizer**
* **affix_allocator**
* **bitmapped_block**

Allocators which maybe supplemented in the future:

* **callback_allocator**
* **traced_allocator**

##Usage##

All header files composing on this library are located in **src/include/boost/memory**. It is suffices to clone this repository and add **src/include** to the list of include directories minded during compilation of your project. For example look into [CMakeLists.txt from composition sample](https://bitbucket.org/lukaszlaszko/allocators/raw/HEAD/samples/composition/CMakeLists.txt).

The most convenient way of using the library is presented in provided sample applications under **samples** directory. Thus including a single header file **boost/memory/allocators.hpp** and using following macros:

* **DEFINE_ALLOCATOR** which should be instantiated in the declaration space of the main source file of the application in which we wish to replace default **new** and **delete** operators with our own, using supplied compassable allocator as a replacement for the default c++ allocation strategy. Should we trace use of the custom allocator through the overridden operator **DEFINE_ALLOCATOR_WITH_TRACE** macro has to be used instead.

* **INIT_ALLOCATOR** this macro should be instantiated at the very start of main function of every thread in the application. The will properly instantiated dependent allocator. Until instantiated, **mallocator** is used for allocations/deallocations through **new**/**delete** in that thread. **TEARDOWN_ALLOCATOR** can be used to explicitly teardown the allocator used in a thread.   

##Build##

The library is purely header based, as such it doesn't require compilation for use in third party project. Thus use of c++ 14 compatible compiler is necessary. 

In order to build unit tests and samples, cmake (3.2 or newer), internet access and all google tests framework prerequisites are required. Any C++ 14 compatible compiler should do, tho the project has been successfully build and tested only on OS X (both Xcode and clang) and Linux (Ubuntu 16.04) with clang 3.9.

To build:

1. Clone this repository

        #!shell
        $ git clone https://bitbucket.org/lukaszlaszko/allocators.git 

2. Configure with cmake
    
        #!shell
        $ mkdir bin
        $ cd bin
        $ cmake ..

3. Build with cmake

        #!shell
        $ cmake --build . --target all

4. Run unit tests

        #!shell
        $ ctest --verbose

Latest bitbucket build results can be observed under this [link](https://bitbucket.org/lukaszlaszko/allocators/addon/pipelines/home#!/).