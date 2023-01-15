cmake_minimum_required (VERSION 3.25)

project("Plugins")

include(Plugins.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
get_winrt(CPPWINRT_HEADERS)

#set properties
add_library(${PROJECT_NAME} STATIC)

set_target_properties(
	${PROJECT_NAME} PROPERTIES
	PLUGINS_DIR ${CMAKE_CURRENT_BINARY_DIR}
)
target_compile_options(${PROJECT_NAME} PRIVATE "/bigobj")

target_sources(${PROJECT_NAME}
	PUBLIC FILE_SET CXX_MODULES
	BASE_DIRS ${CPPWINRT_HEADERS}
	FILES "${CPPWINRT_HEADERS}/winrt/winrt.ixx"
)

install(TARGETS "${PROJECT_NAME}" EXPORT PluginsTargets
    FILE_SET CXX_MODULES DESTINATION modules
    FILE_SET HEADERS DESTINATION include
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(EXPORT PluginsTargets
     FILE "PluginsTargets.cmake"
     NAMESPACE WinD3D::
     DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT PluginsTargets
       FILE "PluginsTargets.cmake"
       NAMESPACE WinD3D::
)