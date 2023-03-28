cmake_minimum_required (VERSION 3.22)

project(DXTEX)


if(NOT PLUGINS_LOADED)
    message("Setting up DirectXTex...")
    include(SetupDirectXTex.cmake)
endif()


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
     NAMESPACE Ver::
     DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT DXTEXTargets
       FILE "DXTEXTargets.cmake"
       NAMESPACE Ver::
)