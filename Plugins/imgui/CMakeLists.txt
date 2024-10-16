cmake_minimum_required (VERSION 3.22)

project(imgui)

conan_cmake_configure(REQUIRES imgui/cci.20220621+1.88.docking
                      GENERATORS cmake_find_package
                      IMPORTS 
                      "./res/bindings, imgui_impl_dx11.cpp -> bindings"  
                      "./res/bindings, imgui_impl_dx11.h -> bindings"  
                      "./res/bindings, imgui_impl_win32.cpp -> bindings"  
                      "./res/bindings, imgui_impl_win32.h -> bindings"  
)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
find_package(imgui REQUIRED)

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} 
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/bindings/imgui_impl_dx11.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/bindings/imgui_impl_win32.cpp
    PUBLIC FILE_SET HEADERS 
        BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES 
            ${CMAKE_CURRENT_BINARY_DIR}/bindings/imgui_impl_dx11.h
            ${CMAKE_CURRENT_BINARY_DIR}/bindings/imgui_impl_win32.h
)

target_include_directories(${PROJECT_NAME} SYSTEM BEFORE 
    INTERFACE         
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)

install(TARGETS "${PROJECT_NAME}" EXPORT imguiTargets
    FILE_SET HEADERS DESTINATION include
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)
install(EXPORT imguiTargets
     FILE "imguiTargets.cmake"
     NAMESPACE WinD3D::
     DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT imguiTargets
       FILE "imguiTargets.cmake"
       NAMESPACE WinD3D::
)