# CMakeList.txt : CMake project for Unreal, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.25)

project("Core" CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


set(DIRECTX_ARM FALSE)
if(DEFINED VCPKG_TARGET_ARCHITECTURE)
    set(DIRECTX_ARCH ${VCPKG_TARGET_ARCHITECTURE})
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Ww][Ii][Nn]32$")
    set(DIRECTX_ARCH x86)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Xx]64$")
    set(DIRECTX_ARCH x64)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Aa][Rr][Mm]$")
    set(DIRECTX_ARCH arm)
    set(DIRECTX_ARM TRUE)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Aa][Rr][Mm]64$")
    set(DIRECTX_ARCH arm64)
    set(DIRECTX_ARM TRUE)
endif()


set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library (${PROJECT_NAME} STATIC)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
        $<BUILD_INTERFACE:${include_dir}>
        $<INSTALL_INTERFACE:include>
)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${include_dir}/pch.h)

#Floating point architecture
if(NOT ${DIRECTX_ARM})
    target_compile_options(${PROJECT_NAME} PUBLIC /arch:AVX2)
endif()

target_link_libraries(${PROJECT_NAME} 
	PUBLIC 
		Shared 
		d3d12 
) 

target_sources(${PROJECT_NAME}
	PRIVATE 
		${source_dir}/Graphics.cpp
        ${include_dir}/pch.h
	PUBLIC FILE_SET HEADERS 
        BASE_DIRS ${include_dir}
        FILES 
		    ${include_dir}/Core/Graphics.h
)

install(TARGETS "${PROJECT_NAME}" EXPORT CoreTargets
    FILE_SET CXX_MODULES DESTINATION modules
    FILE_SET HEADERS DESTINATION include
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(EXPORT CoreTargets
     FILE "CoreTargets.cmake"
     NAMESPACE WinD3D::
     DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT CoreTargets
       FILE "CoreTargets.cmake"
       NAMESPACE WinD3D::
)
