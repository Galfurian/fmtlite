cmake_minimum_required(VERSION 3.10)
project(fmtlite LANGUAGES C CXX)

enable_testing()

# Library
add_library(fmtlite STATIC src/fmtlite.c)
target_include_directories(fmtlite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Test executable
add_executable(test_fmtlite tests/test_fmtlite.c)
target_link_libraries(test_fmtlite PRIVATE fmtlite)
add_test(NAME fmtlite_test COMMAND test_fmtlite)

add_executable(test_fmtlite_cpp tests/test_fmtlite.cpp)
target_link_libraries(test_fmtlite_cpp PRIVATE fmtlite)
set_target_properties(test_fmtlite_cpp PROPERTIES LINKER_LANGUAGE CXX)
add_test(NAME test_fmtlite_cpp COMMAND test_fmtlite_cpp)
