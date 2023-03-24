# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (Test CXX RC)

list(APPEND CMAKE_MODULE_PATH ${PLUGINS_DIR})


add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
	PRIVATE
		"src/EntryMain.cpp"
	PUBLIC 
)

target_link_libraries(${PROJECT_NAME} 
	PUBLIC 
		Ver::WinD3D12
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_precompile_headers(${PROJECT_NAME} PRIVATE "include/pch.h")
set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD 23
)