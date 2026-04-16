cmake_minimum_required (VERSION 3.24)

project ("Veritas")
include(cmake/conan.cmake)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>")
endif()


# Include sub-projects.
add_subdirectory ("lib")
add_subdirectory ("Game")
