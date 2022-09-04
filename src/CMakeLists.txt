add_library(cl_framework STATIC)

file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB includes ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
list(APPEND includes ${CMAKE_CURRENT_SOURCE_DIR}/env/env.hpp)

if(MSVC)
  message(STATUS "we are under windows")  
  list(APPEND sources ${CMAKE_CURRENT_SOURCE_DIR}/env/env_windows.cpp)
  target_include_directories(cl_framework
    PUBLIC 
    $ENV{INTELOCLSDKROOT}include
  )
  target_link_libraries(cl_framework
    PUBLIC $ENV{INTELOCLSDKROOT}lib/x86/OpenCL.lib
  )
elseif(UNIX)
  # assuming here openCL headers are installed under linux
  message(STATUS "we are under linux")  
  list(APPEND sources ${CMAKE_CURRENT_SOURCE_DIR}/env/env_linux.cpp)
  find_package(OpenCL REQUIRED)
  target_link_libraries(cl_framework
    PUBLIC OpenCL::OpenCL
  )
elseif(WIN32)
  message(STATUS "win32????")
endif()

message(STATUS "cl_framework sources: ${sources}")
message(STATUS "cl_framework includes: ${includes}")

target_sources(cl_framework
  PUBLIC 
  ${sources}
  ${includes}  
)