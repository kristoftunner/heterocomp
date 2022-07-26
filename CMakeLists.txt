cmake_minimum_required(VERSION 3.12)

add_executable(cl_main "")
add_subdirectory(src)

target_include_directories(cl_main
  PRIVATE
  $ENV{INTELOCLSDKROOT}include
)

target_link_libraries(cl_main
  PRIVATE $ENV{INTELOCLSDKROOT}lib/x86/OpenCL.lib
)
