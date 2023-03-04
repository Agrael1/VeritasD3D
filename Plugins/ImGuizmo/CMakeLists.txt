cmake_minimum_required (VERSION 3.22)

project(imguizmo)

FetchContent_Declare(
  vimguizmo
  GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
  GIT_TAG origin/master
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)
FetchContent_GetProperties(vimguizmo)
if(NOT vimguizmo_POPULATED)
  FetchContent_Populate(vimguizmo)
endif()

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME}
	PRIVATE
		${vimguizmo_SOURCE_DIR}/ImGuizmo.cpp
	PUBLIC FILE_SET HEADERS 
        BASE_DIRS ${vimguizmo_SOURCE_DIR}
        FILES 
		    ${vimguizmo_SOURCE_DIR}/ImGuizmo.h
)
target_include_directories(${PROJECT_NAME}
    PUBLIC         
        $<BUILD_INTERFACE:${vimguizmo_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD 23
)

install(TARGETS "${PROJECT_NAME}" EXPORT imguizmoTargets
    FILE_SET HEADERS DESTINATION include
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)
install(EXPORT imguizmoTargets
     FILE "imguizmoTargets.cmake"
     NAMESPACE WinD3D::
     DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT imguizmoTargets
       FILE "imguizmoTargets.cmake"
       NAMESPACE WinD3D::
)