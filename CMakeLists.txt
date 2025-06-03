cmake_minimum_required(VERSION 3.10)

project(audiodevice-test VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)

add_executable(main
  main.cc
)

target_link_libraries(main audiodevice)
