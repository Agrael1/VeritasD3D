cmake_minimum_required (VERSION 3.24)

project ("Veritas")

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>")
endif()

# Include sub-projects.
#add_subdirectory ("Plugins")
#add_subdirectory ("Shared")
#add_subdirectory ("Shaders")
#add_subdirectory ("PhysX")
add_subdirectory ("WinD3D")
add_subdirectory ("Game")
