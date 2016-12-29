include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})

file(GLOB ALLOCATORS_HEADERS ${CMAKE_SOURCE_DIR}/src/include/*)
add_executable(tests
        ${ALLOCATORS_HEADERS}
        alignment_tests.cpp
        null_allocator_tests.cpp
        fallback_allocator_tests.cpp
        mallocator_tests.cpp
        stack_allocator_tests.cpp
        free_list_tests.cpp
        segregator_tests.cpp
        bucketizer_tests.cpp
        affix_allocator_tests.cpp)

target_link_libraries(tests
        ${GTEST_LIBRARIES}
        ${GMOCK_LIBRARIES})

add_dependencies(tests 
        ${GTEST_DEPENDENCY})

add_test(NAME run-tests COMMAND tests)
