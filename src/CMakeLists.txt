cmake_minimum_required(VERSION 3.24)
project(annadb_driver)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall \
                     -Wextra \
                     -Werror \
                     -Wpedantic \
                     -Wshadow \
                     -Wnon-virtual-dtor \
                     -Wold-style-cast \
                     -Wcast-align \
                     -Wfloat-conversion \
                     -fno-omit-frame-pointer \
                     -fsanitize=address")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS NO)

if ("$ENV{TESTING}")
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    )

    FetchContent_MakeAvailable(googletest)
    include_directories(${GTEST_INCLUDE_DIRS})

    add_executable(annadb_driver tests/testmain.cpp connection.hpp TySON.hpp)
    target_link_libraries(annadb_driver gtest_main)

    include(GoogleTest)
    gtest_discover_tests(annadb_driver)

else ()
    find_package(cppzmq)

    add_executable(annadb_driver main.cpp connection.hpp TySON.hpp)
    target_link_libraries(annadb_driver cppzmq)
endif ()
