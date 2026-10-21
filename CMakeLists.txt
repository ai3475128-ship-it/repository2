cmake_minimum_required(VERSION 3.15)
project(matrix_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# библиотека: её описание лежит в её собственном репозитории
add_subdirectory(linalg)

# GoogleTest скачивается при конфигурации
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(matrix_tests source/tests.cpp)
target_link_libraries(matrix_tests PRIVATE linalg GTest::gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(matrix_tests)