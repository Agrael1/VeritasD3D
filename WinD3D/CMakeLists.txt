# CMakeList.txt : CMake project for Unreal, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.22)

project(WinD3D CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


list(APPEND CMAKE_MODULE_PATH ${PLUGINS_DIR})
find_package(assimp REQUIRED)
find_package(imgui REQUIRED)

add_subdirectory ("shaders")

set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(VENGINE_HEADERS ${include_dir} PARENT_SCOPE)

file(GLOB_RECURSE vhead "${include_dir}/*/*.h")
file(GLOB_RECURSE vsrc "${source_dir}/*.cpp")


add_library (${PROJECT_NAME} STATIC ${vhead} ${vsrc} cpp.hint)
add_custom_target(copy_assets ALL 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/assets
	${CMAKE_BINARY_DIR}/assets
	COMMENT "Copying assets to binary directory")


if(MSVC)
  target_compile_options(${PROJECT_NAME} PUBLIC "/ZI")
  target_link_options(${PROJECT_NAME} PUBLIC "/INCREMENTAL")
endif()

include_directories(BEFORE SYSTEM ${CMAKE_BINARY_DIR}/include)
target_compile_options(${PROJECT_NAME} PUBLIC /bigobj)
target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG_MODE=$<IF:$<CONFIG:Debug>,1,0>)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/imgui")
target_include_directories(${PROJECT_NAME} PUBLIC ${include_dir})
target_include_directories(${PROJECT_NAME} PUBLIC ${WIL_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${assimp_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${DXMISSING_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${DXTEX_HEADERS})
target_precompile_headers(${PROJECT_NAME} PUBLIC ${include_dir}/pch.h)
target_link_libraries(${PROJECT_NAME} PUBLIC d3d11 dxguid D3DCompiler.lib) 
target_link_libraries(${PROJECT_NAME} PUBLIC ${DXTEX_LIB}) 
target_link_libraries(${PROJECT_NAME} PUBLIC ${DXMISSING_LIB}) 
target_link_libraries(${PROJECT_NAME} PUBLIC assimp::assimp) 
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui) 
target_link_libraries(${PROJECT_NAME} PUBLIC ttf2mesh) 
add_dependencies(${PROJECT_NAME} shaders copy_assets)


