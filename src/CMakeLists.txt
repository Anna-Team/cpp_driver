cmake_minimum_required(VERSION 3.24)
project(annadb_driver VERSION 1.0 LANGUAGES CXX)

find_package(cppzmq)

if ("$ENV{TESTING}")
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    )

    FetchContent_MakeAvailable(googletest)
    include_directories(${GTEST_INCLUDE_DIRS})

    add_executable(annadb_driver
            tests/testmain.cpp
            connection.hpp
            TySON.hpp
            tests/test_tyson_parsing.cpp
            tests/test_connection_data.cpp tests/test_query_creating.cpp tests/test_comparator.cpp)
    target_link_libraries(annadb_driver gtest_main cppzmq)

    include(GoogleTest)
    gtest_discover_tests(annadb_driver)

else ()
    add_executable(annadb_driver main.cpp connection.hpp TySON.hpp utils.hpp query.hpp query_comparision.hpp)
    target_link_libraries(annadb_driver cppzmq)
endif ()


if ("$ENV{GCC}")
    target_compile_options(annadb_driver PRIVATE
                                         -Wall
                                         -Wextra
                                         -Werror
                                         -Wpedantic
                                         -Wshadow
                                         -Wnon-virtual-dtor
                                         -Wold-style-cast
                                         -Wcast-align
                                         -Wfloat-conversion
                                         -fno-omit-frame-pointer
                                         -fsanitize=address
                           )
    target_link_options(annadb_driver PRIVATE -fsanitize=address)
elseif("$ENV{CLANG}")
    target_compile_options(annadb_driver PRIVATE
                                         -Wall
                                         -Wpedantic)
endif()

target_compile_features(annadb_driver PUBLIC cxx_std_20)
set_target_properties(annadb_driver PROPERTIES
                                    CXX_STANDARD 20
                                    CXX_STANDARD_REQUIRED YES
                                    CXX_EXTENSIONS NO)
