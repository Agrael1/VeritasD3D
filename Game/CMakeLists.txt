# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (UnrealTournament CXX RC)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

list(APPEND CMAKE_MODULE_PATH ${PLUGINS_DIR})
find_package(vorbis REQUIRED)



set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE vhead "${include_dir}/*/*.h")
file(GLOB_RECURSE vsrc "${source_dir}/*.cpp")
file(GLOB bind "${CMAKE_BINARY_DIR}/imgui/bindings/*.cpp")

add_executable(${PROJECT_NAME} ${vhead} ${vsrc} ${bind} "${CMAKE_CURRENT_SOURCE_DIR}/resources/WinD3D.rc" "src/Audio/buffer_stream.cpp")

if(MSVC)
  target_compile_options(${PROJECT_NAME} PUBLIC "/ZI")
  target_link_options(${PROJECT_NAME} PUBLIC "/INCREMENTAL")
endif()

add_custom_target(copy_models ALL 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/models
	${CMAKE_BINARY_DIR}/models
	COMMENT "Copying models to binary directory")
add_custom_target(copy_music ALL 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/music
	${CMAKE_BINARY_DIR}/music
	COMMENT "Copying music to binary directory")

include_directories(BEFORE SYSTEM ${CMAKE_BINARY_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC WinD3D)
target_link_libraries(${PROJECT_NAME} PUBLIC PhysX)
target_link_libraries(${PROJECT_NAME} PUBLIC Vorbis::Vorbis)
target_include_directories(${PROJECT_NAME} PUBLIC ${vorbis_INCLUDES})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/imgui")
target_include_directories(${PROJECT_NAME} PRIVATE ${include_dir})
target_include_directories(${PROJECT_NAME} PRIVATE ${VENGINE_HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${WIL_HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${PHYSICS_HEADERS})
target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/pch.h")
add_dependencies(${PROJECT_NAME} copy_models copy_music)