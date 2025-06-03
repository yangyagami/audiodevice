cmake_minimum_required(VERSION 3.10)

project(audiodevice VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME} SHARED
  audiodevice.cc
  audiodevice_creator.cc
  alsa_backend/alsa_audio_device.cc
)
target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${PROJECT_SOURCE_DIR}
)

target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall -Wextra
  $<$<CONFIG:Debug>:-O0>
  $<$<CONFIG:Release>:-O2>
)

target_link_libraries(${PROJECT_NAME}
  asound
  $<$<CONFIG:Debug>:-fsanitize=address>
)
  
