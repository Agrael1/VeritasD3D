cmake_minimum_required (VERSION 3.25)

project("Plugins")

include(Plugins.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
get_winrt(CPPWINRT_HEADERS)

#set properties
add_library(${PROJECT_NAME} INTERFACE)

set_target_properties(
	${PROJECT_NAME} PROPERTIES
	PLUGINS_DIR ${CMAKE_CURRENT_BINARY_DIR}
	WINRT_HEADERS ${CPPWINRT_HEADERS}
)
target_include_directories(${PROJECT_NAME} SYSTEM BEFORE 
    INTERFACE         
        $<BUILD_INTERFACE:${CPPWINRT_HEADERS}>
        $<INSTALL_INTERFACE:include>
)

install(TARGETS "${PROJECT_NAME}" EXPORT PluginsTargets DESTINATION include)

install(EXPORT PluginsTargets
     FILE "PluginsTargets.cmake"
     NAMESPACE WinD3D::
     DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT PluginsTargets
       FILE "PluginsTargets.cmake"
       NAMESPACE WinD3D::
)