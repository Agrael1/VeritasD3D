# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (Test2 CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${include_dir})
target_link_libraries(${PROJECT_NAME} PRIVATE Core)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${include_dir}/pch.h)

target_sources(${PROJECT_NAME}
	PRIVATE
		${source_dir}/Game/App.cpp
		${source_dir}/EntryMain.cpp
)