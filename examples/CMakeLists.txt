cmake_minimum_required(VERSION 3.24)
project(annadb_driver_example VERSION 1.0 LANGUAGES CXX)

find_package(cppzmq REQUIRED)

add_executable(annadb_driver_example main.cpp ../src/connection.hpp ../src/TySON.hpp ../src/utils.hpp ../src/query.hpp ../src/query_comparision.hpp)
target_link_libraries(annadb_driver_example cppzmq)

target_compile_options(annadb_driver_example PRIVATE
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
target_link_options(annadb_driver_example PRIVATE -fsanitize=address)

target_compile_features(annadb_driver_example PUBLIC cxx_std_20)
set_target_properties(annadb_driver_example PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
