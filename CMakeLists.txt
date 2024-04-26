cmake_minimum_required(VERSION 3.29)
project(Smot)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(${PROJECT_NAME} src/Smot.cpp)

target_link_libraries(${PROJECT_NAME} X11)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror 
  -Wno-error=unused-variable -Wno-error=unused-parameter)
