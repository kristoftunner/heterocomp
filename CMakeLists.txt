cmake_minimum_required(VERSION 3.12)
project(heterocomp CXX)
add_library(cl_main STATIC)

add_subdirectory(src)

if(MSVC)
  message(STATUS "we are under windows")  
  target_include_directories(cl_main
    PUBLIC 
    $ENV{INTELOCLSDKROOT}include
  )
  target_link_libraries(cl_main
    PUBLIC $ENV{INTELOCLSDKROOT}lib/x86/OpenCL.lib
  )
elseif(UNIX)
  # assuming here openCL headers are installed under linux
  message(STATUS "we are under linux")  
  find_package(OpenCL REQUIRED)
  target_link_libraries(cl_main
    PUBLIC OpenCL::OpenCL
  )
elseif(WIN32)
  message(STATUS "win32????")
endif()

add_executable(sandbox 
  src/sandbox_app.cpp
)

target_link_libraries(sandbox
  PRIVATE cl_main
)