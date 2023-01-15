# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.22)

project ("Unreal")

include(FetchContent)

# Enable Hot Reload if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>")
endif()

#set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "3c375311-a3c9-4396-a187-3227ef642046")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 0)

# Include sub-projects.
add_subdirectory ("Plugins")

get_target_property(PLUGINS_DIR "Plugins" PLUGINS_DIR)
list(APPEND CMAKE_MODULE_PATH ${PLUGINS_DIR})

#add_subdirectory ("PhysX")
#add_subdirectory ("ttf2mesh")
add_subdirectory ("WinD3D")
#add_subdirectory ("Game")
add_subdirectory ("Game2")
