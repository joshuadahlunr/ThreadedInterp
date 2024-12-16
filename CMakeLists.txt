cmake_minimum_required(VERSION 3.20)
project(MizuVM CXX C)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(ffi/libffi)

add_executable(tst "test.cpp")
target_link_libraries(tst PUBLIC ffi_static)

add_library(tst_load SHARED shared.cpp)