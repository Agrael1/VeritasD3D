cmake_minimum_required (VERSION 3.22)

project(DXTEX)

find_program(NUGET_EXE NAMES nuget PATHS ${CMAKE_SOURCE_DIR}/Plugins/NuGet)
if(NOT NUGET_EXE)
	message("NUGET.EXE not found.")
	message(FATAL_ERROR "Please install this executable, and run CMake again.")
endif()

exec_program(${NUGET_EXE}
    ARGS install "directxtex_desktop_win10" -OutputDirectory ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB DXTEX_DIR ${CMAKE_CURRENT_BINARY_DIR}/directxtex_desktop_win10.*)
set(DXTEX_LIB ${DXTEX_DIR}/native/lib/x64/$<IF:$<CONFIG:Debug>,Debug,Release>/DirectXTex.lib)


add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} SYSTEM BEFORE 
    INTERFACE         
        $<BUILD_INTERFACE:${DXTEX_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(${PROJECT_NAME} INTERFACE
    ${DXTEX_LIB}
)

install(TARGETS "${PROJECT_NAME}" EXPORT DXTEXTargets DESTINATION include)
install(EXPORT DXTEXTargets
     FILE "DXTEXTargets.cmake"
     NAMESPACE WinD3D::
     DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT DXTEXTargets
       FILE "DXTEXTargets.cmake"
       NAMESPACE WinD3D::
)