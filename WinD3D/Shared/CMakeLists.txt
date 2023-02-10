# CMakeList.txt : CMake project for Unreal, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.25)
project("Shared" CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 0)



find_package(spdlog REQUIRED)

add_library (${PROJECT_NAME} STATIC)

set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(module_dir "${CMAKE_CURRENT_SOURCE_DIR}/modules")

#get_target_property(WINRT_HEADERS Plugins WINRT_HEADERS)
#target_include_directories(${PROJECT_NAME} SYSTEM BEFORE PUBLIC $<BUILD_INTERFACE:${WINRT_HEADERS}> $<INSTALL_INTERFACE:include>)
target_include_directories(${PROJECT_NAME}
    PUBLIC 
        $<BUILD_INTERFACE:${include_dir}>
        $<INSTALL_INTERFACE:include>
)
target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG_MODE=$<IF:$<CONFIG:Debug>,1,0>)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog Plugins dxguid DXGI)
#target_precompile_headers(${PROJECT_NAME} PRIVATE ${include_dir}/pch.h)

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
            "include/Shared/Definitions.h"            
            "include/Shared/Timer.h"            
            include/Shared/Log.h
 "include/Shared/Profile.h" )

get_target_property(dirs ${PROJECT_NAME} INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

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