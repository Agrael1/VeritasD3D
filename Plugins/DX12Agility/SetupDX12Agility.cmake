find_program(NUGET_EXE NAMES nuget PATHS ${CMAKE_SOURCE_DIR}/Plugins/NuGet)
if(NOT NUGET_EXE)
	message("NUGET.EXE not found.")
	message(FATAL_ERROR "Please install this executable, and run CMake again.")
endif()

exec_program(${NUGET_EXE}
    ARGS install "Microsoft.Direct3D.D3D12" -OutputDirectory ${CMAKE_CURRENT_BINARY_DIR})



file(GLOB DXA_DIRS ${CMAKE_CURRENT_BINARY_DIR}/Microsoft.Direct3D.D3D12.*)
list(LENGTH DXA_DIRS DXA_DIRS_L)
if(${DXA_DIRS_L} GREATER 1)
    #Sort directories by version in descending order, so the first dir is top version
    list(SORT DXA_DIRS COMPARE NATURAL ORDER DESCENDING)
    list(GET DXA_DIRS 0 DXA_DIRX)

    #Remove older version
    MATH(EXPR DXA_DIRS_L "${DXA_DIRS_L}-1")
    foreach(I RANGE 1 ${DXA_DIRS_L})
        list(GET DXA_DIRS ${I} OLD)
        file(REMOVE_RECURSE ${OLD})
    endforeach()
else()
    list(GET DXA_DIRS 0 DXA_DIRX)
endif()

#set cache
set(DXA_DIR ${DXA_DIRX} CACHE INTERNAL "")