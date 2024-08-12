# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (UnrealTournament CXX RC)

list(APPEND CMAKE_MODULE_PATH ${PLUGINS_DIR})
find_package(vorbis REQUIRED)


add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
	PRIVATE
		"resources/WinD3D.rc"

		"src/EntryMain.cpp"
		"src/Audio/Audio.cpp"
		"src/Audio/buffer_stream.cpp"
		"src/Game/App.cpp"
		"src/Game/Camera.cpp"
		"src/Game/Flag.cpp"
		"src/Game/Interaction.cpp"
		"src/Game/Keyboard.cpp"
		"src/Game/Level.cpp"
		"src/Game/MediaEngine.cpp"
		"src/Game/Mouse.cpp"
		"src/Game/Player.cpp"
		"src/Game/PointLight.cpp"
		"src/Game/Portal.cpp"
		"src/Game/Window.cpp"
	PUBLIC 
		"include/Audio/Audio.h"
		"include/Audio/audio_ptr.h"
		"include/Audio/buffer_stream.h"
		"include/Audio/vorbis_file.h"

		"include/Game/App.h"
		"include/Game/Camera.h"
		"include/Game/Entity.h"
		"include/Game/Flag.h"
		"include/Game/Interaction.h"
		"include/Game/Keyboard.h"
		"include/Game/Level.h"
		"include/Game/Loading.h"
		"include/Game/MediaEngine.h"
		"include/Game/Mouse.h"
		"include/Game/Pictures.h"
		"include/Game/Player.h"
		"include/Game/PointLight.h"
		"include/Game/Portal.h"
		"include/Game/Video.h"
		"include/Game/Window.h"

		"include/Util/Converter.h"
		"include/Util/Filters.h"
		"include/Game/Menu.h" 
		"include/Game/Grid.h"
)


add_custom_target(copy_models ALL 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/models
	${CMAKE_BINARY_DIR}/models
	COMMENT "Copying models to binary directory")
add_custom_target(copy_music ALL 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/music
	${CMAKE_BINARY_DIR}/music
	COMMENT "Copying music to binary directory"
)

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${HBAO_DYLIB} $<TARGET_FILE_DIR:${PROJECT_NAME}>
#  COMMAND_EXPAND_LISTS
#  )
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
  COMMAND_EXPAND_LISTS
  COMMENT "Copying Dependencies..."
)

target_link_libraries(${PROJECT_NAME} 
	PUBLIC 
		WinD3D 
		PhysX 
		Vorbis::Vorbis 
		Mfplat
		runtimeobject.lib
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_precompile_headers(${PROJECT_NAME} PRIVATE "include/pch.h")
set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD 23
)
add_dependencies(${PROJECT_NAME} copy_models copy_music)