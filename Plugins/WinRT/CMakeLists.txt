cmake_minimum_required (VERSION 3.22)

project(CPPWinRT)

find_program(NUGET_EXE NAMES nuget PATHS ${CMAKE_SOURCE_DIR}/Plugins/NuGet)
if(NOT NUGET_EXE)
	message("NUGET.EXE not found.")
	message(FATAL_ERROR "Please install this executable, and run CMake again.")
endif()

exec_program(${NUGET_EXE}
    ARGS install "Microsoft.Windows.CppWinRT" -OutputDirectory ${CMAKE_CURRENT_BINARY_DIR})



file(GLOB CPPWINRT_DIRS ${CMAKE_CURRENT_BINARY_DIR}/Microsoft.Windows.CppWinRT.*)
list(LENGTH CPPWINRT_DIRS CPPWINRT_DIRS_L)
if(${CPPWINRT_DIRS_L} GREATER 1)
    #Sort directories by version in descending order, so the first dir is top version
    list(SORT CPPWINRT_DIRS COMPARE NATURAL ORDER DESCENDING)
    list(GET CPPWINRT_DIRS 0 CPPWINRT_DIR)

    #Remove older version
    MATH(EXPR CPPWINRT_DIRS_L "${CPPWINRT_DIRS_L}-1")
    foreach(I RANGE 1 ${CPPWINRT_DIRS_L})
        list(GET CPPWINRT_DIRS ${I} OLD)
        file(REMOVE_RECURSE ${OLD})
    endforeach()
else()
    list(GET CPPWINRT_DIRS 0 CPPWINRT_DIR)
endif()
set(CPPWINRT ${CPPWINRT_DIR}/bin/cppwinrt.exe)
set(CPPWINRT_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/include)

execute_process(COMMAND
    ${CPPWINRT} -input sdk -output include
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    RESULT_VARIABLE ret)
if (NOT ret EQUAL 0)
    message(FATAL_ERROR "Failed to run cppwinrt.exe ${CPPWINRT_DIR} ${ret}")
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
     NAMESPACE WinD3D::
     DESTINATION "lib/cmake/${PROJECT_NAME}"
)

export(EXPORT WinRTTargets
       FILE "WinRTTargets.cmake"
       NAMESPACE WinD3D::
)