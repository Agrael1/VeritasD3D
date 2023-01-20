# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (Test2 CXX RC)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")

#WIL
FetchContent_Declare(
  wil
  GIT_REPOSITORY https://github.com/microsoft/wil.git
  GIT_TAG origin/master
)
FetchContent_GetProperties(wil)
if(NOT wil_POPULATED)
  set(WIL_BUILD_PACKAGING OFF)
  set(WIL_BUILD_TESTS OFF)
  FetchContent_Populate(wil)
endif()
FetchContent_MakeAvailable(wil)



add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} 
	PRIVATE 
		${include_dir} 
		"${CMAKE_CURRENT_SOURCE_DIR}/resources"
		${wil_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE Core)
#target_precompile_headers(${PROJECT_NAME} PRIVATE ${include_dir}/pch.h)

target_sources(${PROJECT_NAME}
	PUBLIC
		${include_dir}/Game/Window.h
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/resources/WinD3D.rc"

		${source_dir}/Game/App.cpp
		${source_dir}/Game/Window.cpp
		${source_dir}/EntryMain.cpp
 "include/Game/Keyboard.h" "include/Game/Mouse.h")