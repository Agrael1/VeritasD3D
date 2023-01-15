# CMakeList.txt : CMake project for Unreal, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.25)
project("Shared" CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(spdlog REQUIRED)

add_library (${PROJECT_NAME} STATIC)

set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(module_dir "${CMAKE_CURRENT_SOURCE_DIR}/modules")

target_include_directories(${PROJECT_NAME} 
    INTERFACE 
        ${spdlog_INCLUDES}
    PUBLIC 
    "$<BUILD_INTERFACE:${include_dir}>"
    "$<INSTALL_INTERFACE:include>"
)
target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG_MODE=$<IF:$<CONFIG:Debug>,1,0>)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog Plugins)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        ${source_dir}/Checks.cpp
        ${source_dir}/DXGIInfoManager.cpp
        ${source_dir}/Exception.cpp
        ${source_dir}/Log.cpp
    PUBLIC FILE_SET HEADERS 
        BASE_DIRS ${include_dir}
        FILES 
            include/Shared/Checks.h 
            include/Shared/DXGIInfoManager.h 
            include/Shared/Exception.h 
            include/Shared/Severity.h 
            "include/Shared/Defines.h"            
            "include/Shared/Timer.h"            
            include/Shared/Log.h
    PUBLIC
        FILE_SET CXX_MODULES 
        FILES 
            "${module_dir}/shared.ixx"
)

install(TARGETS "${PROJECT_NAME}" EXPORT SharedTargets
    FILE_SET CXX_MODULES DESTINATION modules
    FILE_SET HEADERS DESTINATION include
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(EXPORT SharedTargets
     FILE "SharedTargets.cmake"
     NAMESPACE WinD3D::
     DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT SharedTargets
       FILE "SharedTargets.cmake"
       NAMESPACE WinD3D::
)