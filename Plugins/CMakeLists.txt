cmake_minimum_required (VERSION 3.22)


set(PACKAGE_DIR ${CMAKE_BINARY_DIR}/packages)
set(PLUGINS_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})


# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                  "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
include(Misc.cmake)


add_subdirectory("WinRT")
add_subdirectory("Wil")
#add_subdirectory("imgui")
#add_subdirectory("ImGuizmo")
add_subdirectory("DirectXTex")
add_subdirectory("DX12Agility")
add_subdirectory("HBAO")


#Assimp
if(NOT PLUGINS_LOADED)
    message("Loading packages")
    conan_cmake_configure(REQUIRES vorbis/1.3.7 assimp/5.2.2 physx/4.1.1 spdlog/1.11.0
                          GENERATORS cmake_find_package
    )
    
    conan_cmake_autodetect(settings)
    
    conan_cmake_install(PATH_OR_REFERENCE .
                        BUILD missing
                        REMOTE conancenter
                        SETTINGS ${settings}
    )
endif()
