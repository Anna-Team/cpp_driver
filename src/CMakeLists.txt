cmake_minimum_required(VERSION 3.24)
project(annadb_driver VERSION 1.0 LANGUAGES CXX)

if ("$ENV{TESTING}")
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    )

    FetchContent_MakeAvailable(googletest)
    include_directories(${GTEST_INCLUDE_DIRS})

    add_executable(${PROJECT_NAME}
            tests/testmain.cpp
            connection.hpp
            TySON.hpp
            utils.hpp
            query.hpp
            query_comparision.hpp
            tests/test_tyson_parsing.cpp
            tests/test_connection_data.cpp
            tests/test_query_creating.cpp
            tests/test_comparator.cpp
            )

    target_link_libraries(${PROJECT_NAME} gtest_main)

    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME})

else ()
    find_package(cppzmq REQUIRED)
    add_executable(${PROJECT_NAME} main.cpp
                                   connection.hpp
                                   TySON.hpp
                                   utils.hpp
                                   query.hpp
                                   query_comparision.hpp)
    target_link_libraries(${PROJECT_NAME} cppzmq)
endif ()


if ("$ENV{GCC}")
    target_compile_options(${PROJECT_NAME} PRIVATE
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
                                          -fsanitize=address)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
elseif("$ENV{CLANG}")
    target_compile_options(${PROJECT_NAME} PRIVATE
                                           -Wall
                                           -Wpedantic
                                           -fsanitize=address)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES
                                      CXX_STANDARD 20
                                      CXX_STANDARD_REQUIRED YES
                                      CXX_EXTENSIONS NO)
