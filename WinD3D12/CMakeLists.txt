# CMakeList.txt : CMake project for Unreal, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.22)

project(WinD3D12 LANGUAGES CXX VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH ${PLUGINS_DIR})
set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")


add_library (${PROJECT_NAME} STATIC)
add_library (Ver::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE 
		"src/Core/Graphics.cpp"
		"src/x.cpp"
	PUBLIC FILE_SET HEADERS 
		BASE_DIRS ${include_dir}
		FILES 
			"include/Core/Graphics.h"
)


target_link_libraries(${PROJECT_NAME} 
	PUBLIC 
		Ver::Wisdom
) 
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${include_dir}>
  $<INSTALL_INTERFACE:include>
)
target_precompile_headers(${PROJECT_NAME} PUBLIC "include/pch.h")
set_target_properties(${PROJECT_NAME} 
	PROPERTIES
		CXX_STANDARD 23
)

#installation
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	${PROJECT_NAME}ConfigVersion.cmake
	VERSION ${PACKAGE_VERSION}
	COMPATIBILITY AnyNewerVersion
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
		DESTINATION lib/cmake/${PROJECT_NAME}
)
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
	FILE_SET HEADERS DESTINATION include
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	INCLUDES DESTINATION include
)

install(EXPORT ${PROJECT_NAME}Targets
	 FILE "${PROJECT_NAME}Targets.cmake"
	 NAMESPACE Ver::
	 DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT ${PROJECT_NAME}Targets
	   FILE "${PROJECT_NAME}Targets.cmake"
	   NAMESPACE Ver::
)