cmake_minimum_required (VERSION 3.22)

project(PhysX CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

list(APPEND CMAKE_MODULE_PATH ${PLUGINS_DIR})
find_package(PhysX REQUIRED)

set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE vhead "${include_dir}/*.h")
file(GLOB_RECURSE vsrc "${source_dir}/*.cpp")

add_library(${PROJECT_NAME} STATIC ${vhead} ${vsrc} "include/utils.h")

if(MSVC)
  target_compile_options(${PROJECT_NAME} PUBLIC "/ZI")
  target_link_options(${PROJECT_NAME} PUBLIC "/INCREMENTAL")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${physx_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${include_dir})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${include_dir}/pch.h)
target_link_libraries(${PROJECT_NAME} PUBLIC PhysX::PhysX) 
