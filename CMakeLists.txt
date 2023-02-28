# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Unreal")

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

option(DX12_BACKEND "Selects backend for the engine [11/12]" FALSE)

include(FetchContent)

# Include sub-projects.
add_subdirectory ("Plugins")
add_subdirectory ("Shared")
add_subdirectory ("Shaders")
add_subdirectory ("PhysX")
add_subdirectory ("WinD3D")
add_subdirectory ("Game")
