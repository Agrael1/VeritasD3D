cmake_minimum_required (VERSION 3.22)

project(PhysX CXX)

list(APPEND CMAKE_MODULE_PATH ${PLUGINS_DIR})
find_package(PhysX REQUIRED)


add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME}
	PRIVATE 
        "src/Character.cpp"
        "src/Foundation.cpp"
        "src/PhysX.cpp"
        "src/Scene.cpp"
	PUBLIC FILE_SET HEADERS 
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        FILES 
            "include/PhysX/Character.h"
            "include/PhysX/Foundation.h"
            "include/PhysX/Scene.h"
            "include/PhysX/utils.h"
)

target_link_libraries(${PROJECT_NAME} PUBLIC PhysX::PhysX) 
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_precompile_headers(${PROJECT_NAME} PUBLIC "include/pch.h")
set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD 23
)



install(TARGETS "${PROJECT_NAME}" EXPORT PhysXTargets
    FILE_SET HEADERS DESTINATION include
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(EXPORT PhysXTargets
     FILE "PhysXTargets.cmake"
     NAMESPACE WinD3D::
     DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT PhysXTargets
       FILE "PhysXTargets.cmake"
       NAMESPACE WinD3D::
)