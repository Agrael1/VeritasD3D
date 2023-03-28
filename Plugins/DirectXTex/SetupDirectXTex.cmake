find_program(NUGET_EXE NAMES nuget PATHS ${CMAKE_SOURCE_DIR}/Plugins/NuGet)
if(NOT NUGET_EXE)
	message("NUGET.EXE not found.")
	message(FATAL_ERROR "Please install this executable, and run CMake again.")
endif()

exec_program(${NUGET_EXE}
    ARGS install "directxtex_desktop_win10" -OutputDirectory ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB DXTEX_DIRS ${CMAKE_CURRENT_BINARY_DIR}/directxtex_desktop_win10.*)
list(LENGTH DXTEX_DIRS DXTEX_DIRS_L)
if(${DXTEX_DIRS_L} GREATER 1)
    #Sort directories by version in descending order, so the first dir is top version
    list(SORT DXTEX_DIRS COMPARE NATURAL ORDER DESCENDING)
    list(GET DXTEX_DIRS 0 DXTEX_DIRX)

    #Remove older version
    MATH(EXPR DXTEX_DIRS_L "${DXTEX_DIRS_L}-1")
    foreach(I RANGE 1 ${DXTEX_DIRS_L})
        list(GET DXTEX_DIRS ${I} OLD)
        file(REMOVE_RECURSE ${OLD})
    endforeach()
else()
    list(GET DXTEX_DIRS 0 DXTEX_DIRX)
endif()

#set cache
set(DXTEX_DIR ${DXTEX_DIRX} CACHE INTERNAL "")
set(DXTEX_LIB ${DXTEX_DIR}/native/lib/x64/$<IF:$<CONFIG:Debug>,Debug,Release>/DirectXTex.lib  CACHE INTERNAL "")