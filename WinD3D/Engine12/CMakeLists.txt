# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
set(PROJECT "${PROJECT_REF}12")

project (${PROJECT})

# Include sub-projects.
add_subdirectory ("Core")
