cmake_minimum_required(VERSION 2.8)

project(practice1)

set(CMAKE_CXX_FLAGS "-Wall -Werror")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)

add_executable(main
  ${PROJECT_SOURCE_DIR}/src/main.cpp
  ${PROJECT_SOURCE_DIR}/src/solution.cpp
)

add_executable(test
  ${PROJECT_SOURCE_DIR}/src/test.cpp
  ${PROJECT_SOURCE_DIR}/src/solution.cpp
)

target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(test PRIVATE ${PROJECT_SOURCE_DIR}/include)