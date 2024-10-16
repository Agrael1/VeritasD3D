cmake_minimum_required (VERSION 3.22)

project(Shared CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/Plugins)
set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")


find_package(spdlog REQUIRED)

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME}
	PRIVATE 
        "src/DXGIInfoManager.cpp"
        "src/Checks.cpp"
        "src/Exception.cpp"
        "src/Log.cpp"
	 PUBLIC FILE_SET HEADERS 
        BASE_DIRS ${include_dir}
        FILES 
		    "include/Shared/DXGIInfoManager.h"	
            "include/Shared/Definitions.h" 
            "include/Shared/Severity.h" 
            "include/Shared/Checks.h"  
            "include/Shared/Exception.h" 
            "include/Shared/Log.h" 
            "include/Shared/Timer.h" 
            "include/Shared/Profile.h" 
            "include/Shared/WinSetup.h"
)


target_compile_definitions(${PROJECT_NAME} 
    PUBLIC 
        DEBUG_MODE=$<IF:$<CONFIG:Debug>,1,0>
        DX12_BACKEND=$<BOOL:${DX12_BACKEND}>
        NOMINMAX
)
target_link_libraries(${PROJECT_NAME} 
    PUBLIC 
        spdlog::spdlog 
        DXGI
        CPPWinRT
        Wil
) 

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_precompile_headers(${PROJECT_NAME} PUBLIC "include/pch.h")
set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD 23
)



install(TARGETS "${PROJECT_NAME}" EXPORT SharedTargets
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