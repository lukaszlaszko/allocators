cmake_minimum_required(VERSION 3.2)
project(allocators)

# register modules
include(CTest)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 14)
add_definitions(-Wno-unknown-attributes)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pthread")
endif()

# find threads
find_package (Threads)

# add src to includes
include_directories(src/include)

#Setup CMake to run tests
enable_testing()

# include subdirectories
add_subdirectory(external)
add_subdirectory(tests)