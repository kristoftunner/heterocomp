cmake_minimum_required(VERSION 3.12)
project(heterocomp CXX)
include(cmake/defines.cmake)
add_subdirectory(src)

set(CL_FRAMEWORK_INCLUDES ${PROJECT_SOURCE_DIR}/src)
add_subdirectory(tests)