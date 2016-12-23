ExternalProject_Add(googletest
    URL https://github.com/google/googletest/archive/release-1.8.0.tar.gz
    CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=Release
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} 
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
            -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
    BUILD_COMMAND ${CMAKE_COMMAND} --build .
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS
            "${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/googlemock/gtest/libgtest.a" 
            "${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/googlemock/gtest/libgtest_main.a"
            "${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/googlemock/libgmock.a" 
            "${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/googlemock/libgmock_main.a")

# Specify include dir
ExternalProject_Get_Property(googletest SOURCE_DIR)
set(GTEST_INCLUDE_DIRS ${SOURCE_DIR}/googletest/include PARENT_SCOPE)
set(GMOCK_INCLUDE_DIRS ${SOURCE_DIR}/googlemock/include PARENT_SCOPE)

# Specify MainTest's link libraries
ExternalProject_Get_Property(googletest BINARY_DIR)
if (XCODE_VERSION)
    set(GTEST_LIBS_DIR "${BINARY_DIR}/googlemock/gtest/${CMAKE_BUILD_TYPE}")
    set(GMOCK_LIBS_DIR "${BINARY_DIR}/googlemock/${CMAKE_BUILD_TYPE}")
else()
    set(GTEST_LIBS_DIR "${BINARY_DIR}/googlemock/gtest")
    set(GMOCK_LIBS_DIR "${BINARY_DIR}/googlemock")
endif()
set(GTEST_LIBRARIES "${GTEST_LIBS_DIR}/libgtest.a" "${GTEST_LIBS_DIR}/libgtest_main.a" PARENT_SCOPE)
set(GMOCK_LIBRARIES "${GMOCK_LIBS_DIR}/libgmock.a" "${GMOCK_LIBS_DIR}/libgmock_main.a" PARENT_SCOPE)

# specify dependency
set(GTEST_DEPENDENCY googletest PARENT_SCOPE)
