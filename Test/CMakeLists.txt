# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (Test CXX RC)

list(APPEND CMAKE_MODULE_PATH ${PLUGINS_DIR})


add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
	PRIVATE
		"resources/WinD3D.rc"

		"src/EntryMain.cpp"
		"src/Test/Window.cpp" 
		"src/Test/Keyboard.cpp" 
		"src/Test/Mouse.cpp"
	PUBLIC 
 "include/Test/App.h" "include/Test/Keyboard.h" "include/Test/Menu.h" "include/Test/Mouse.h" "include/Test/Window.h" "include/resource.h" "src/Test/App.cpp")

target_link_libraries(${PROJECT_NAME} 
	PUBLIC 
		Ver::WinD3D12
		DX12Agility
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:DX12AgilityCore> $<TARGET_FILE_DIR:${PROJECT_NAME}>/D3D12/$<TARGET_FILE_NAME:DX12AgilityCore>
  COMMAND_EXPAND_LISTS
  COMMENT "Copying DX12 Agility Core..."
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:DX12AgilitySDKLayers> $<TARGET_FILE_DIR:${PROJECT_NAME}>/D3D12/$<TARGET_FILE_NAME:DX12AgilitySDKLayers>
	  COMMAND_EXPAND_LISTS
	  COMMENT "Copying DX12 Agility SDKLayers..."
	)
endif()

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
#  COMMAND_EXPAND_LISTS
#  COMMENT "Copying Dependencies..."
#)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_precompile_headers(${PROJECT_NAME} PRIVATE "include/pch.h")
set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD 23
)