file(GLOB ALLOCATORS_HEADERS ${CMAKE_SOURCE_DIR}/src/include/*)
add_executable(composition-sample
        ${ALLOCATORS_HEADERS}
        main.cpp)

target_link_libraries(composition-sample
        ${GTEST_LIBRARIES}
        ${GMOCK_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})
