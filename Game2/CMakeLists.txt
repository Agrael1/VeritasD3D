# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (Test2 CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE vhead "${include_dir}/*.h")
file(GLOB_RECURSE vsrc "${source_dir}/*.cpp")

add_executable(${PROJECT_NAME} ${vhead} ${vsrc})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PUBLIC "/ZI")
  target_link_options(${PROJECT_NAME} PUBLIC "/INCREMENTAL")
endif()

include_directories(BEFORE SYSTEM ${CMAKE_BINARY_DIR}/include)

target_include_directories(${PROJECT_NAME} PRIVATE ${include_dir})
target_include_directories(${PROJECT_NAME} PRIVATE ${CORE_HEADERS})
target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/pch.h")

target_link_libraries(${PROJECT_NAME} PUBLIC WinD3D12.Core)