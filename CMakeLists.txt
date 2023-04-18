cmake_minimum_required(VERSION 3.0.0)
project(altest VERSION 0.1.0)
include_directories(src)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/)
include(CTest)
enable_testing()

add_executable(altest main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
