file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB includes ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
list(APPEND includes ${CMAKE_CURRENT_SOURCE_DIR}/env/env.hpp)

if(MSVC)
  list(APPEND sources ${CMAKE_CURRENT_SOURCE_DIR}/env/env_windows.cpp)
endif()
message(STATUS "sources: ${sources}")
message(STATUS "includes: ${includes}")

target_sources(cl_main
  PUBLIC 
  ${sources}
  ${includes}  
)
