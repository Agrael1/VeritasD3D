cmake_minimum_required (VERSION 3.22)

project(HBAO)

FetchContent_Declare(
  hbao
  GIT_REPOSITORY https://github.com/NVIDIAGameWorks/HBAOPlus.git
  GIT_TAG origin/master
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)
FetchContent_GetProperties(hbao)
if(NOT hbao_POPULATED)
  FetchContent_Populate(hbao)
endif()

set(HBAO_LIB "${hbao_SOURCE_DIR}/lib/GFSDK_SSAO_D3D11.win64.lib")
set(HBAO_DYLIB "${hbao_SOURCE_DIR}/lib/GFSDK_SSAO_D3D11.win64.dll")


add_library(${PROJECT_NAME} SHARED IMPORTED GLOBAL)
set_property(TARGET ${PROJECT_NAME} PROPERTY
             IMPORTED_LOCATION ${HBAO_DYLIB})
set_property(TARGET ${PROJECT_NAME} PROPERTY
             IMPORTED_IMPLIB ${HBAO_LIB})

target_include_directories(${PROJECT_NAME}
    INTERFACE         
        ${hbao_SOURCE_DIR}/include
)
