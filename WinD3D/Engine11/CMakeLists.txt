# CMakeList.txt : CMake project for Unreal, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.25)
set(PROJECT "${PROJECT_REF}11")

project (${PROJECT} CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library (${PROJECT_NAME} STATIC)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
        $<BUILD_INTERFACE:${include_dir}>
        $<INSTALL_INTERFACE:include>
)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${include_dir}/pch.h)

target_link_libraries(${PROJECT_NAME} 
	PUBLIC 
		Shared 
		d3d12 
) 

target_sources(${PROJECT_NAME}
	PRIVATE 
		${source_dir}/Engine/Graphics.cpp
        #${include_dir}/pch.h
	PUBLIC FILE_SET HEADERS 
        BASE_DIRS ${include_dir}
        FILES 
		    ${include_dir}/Engine/Graphics.h
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
