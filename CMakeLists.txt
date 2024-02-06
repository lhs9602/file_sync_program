cmake_minimum_required(VERSION 3.10)

project(TCP_File_Transfer)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")

set(GTEST_LIBRARY "/usr/local/lib/libgtest.a")
set(GTEST_MAIN_LIBRARY "/usr/local/lib/libgtest_main.a")
set(GTEST_INCLUDE_DIR "/usr/local/include")


find_package(OpenSSL REQUIRED)
find_package(ZLIB  REQUIRED)
find_package(GTest REQUIRED)

include(GoogleTest)

add_subdirectory(src)
add_subdirectory(test)