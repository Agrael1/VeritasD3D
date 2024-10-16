cmake_minimum_required (VERSION 3.22)

project(Wil)

FetchContent_Declare(
  wil
  GIT_REPOSITORY https://github.com/microsoft/wil.git
  GIT_TAG origin/master
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)
FetchContent_GetProperties(wil)
if(NOT wil_POPULATED)
  set(WIL_BUILD_PACKAGING OFF)
  set(WIL_BUILD_TESTS OFF)
  FetchContent_Populate(wil)
endif()


add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} SYSTEM BEFORE 
    INTERFACE         
        $<BUILD_INTERFACE:${wil_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

install(TARGETS "${PROJECT_NAME}" EXPORT WilTargets DESTINATION include)
install(EXPORT WilTargets
     FILE "WilTargets.cmake"
     NAMESPACE WinD3D::
     DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT WilTargets
       FILE "WilTargets.cmake"
       NAMESPACE WinD3D::
)