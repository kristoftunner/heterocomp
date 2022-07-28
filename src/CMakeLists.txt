file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB includes ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

message(STATUS "sources: ${sources}")
message(STATUS "includes: ${includes}")

target_sources(cl_main
  PUBLIC 
  ${sources}
  ${includes}  
)
