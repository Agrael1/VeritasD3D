cmake_minimum_required (VERSION 3.22)

project(CPPWinRT)

if(NOT PLUGINS_LOADED)
    message("Setting up WinRT...")
    include(SetupWinRT.cmake)
endif()


add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} SYSTEM BEFORE 
    INTERFACE         
        $<BUILD_INTERFACE:${CPPWINRT_HEADERS}>
        $<INSTALL_INTERFACE:include>
)

install(TARGETS "${PROJECT_NAME}" EXPORT WinRTTargets DESTINATION include)
install(EXPORT WinRTTargets
     FILE "WinRTTargets.cmake"
     NAMESPACE Ver::
     DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT WinRTTargets
       FILE "WinRTTargets.cmake"
       NAMESPACE Ver::
)