set(PACKAGE_DIR ${CMAKE_BINARY_DIR}/packages)
set(PLUGINS_DIR ${CMAKE_CURRENT_BINARY_DIR})

list(APPEND CMAKE_PREFIX_PATH ${PLUGINS_DIR})


# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
				  "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

#Assimp
conan_cmake_configure(REQUIRES vorbis/1.3.7 assimp/5.2.2 imgui/cci.20220621+1.88.docking physx/4.1.1 spdlog/1.11.0
					  GENERATORS cmake_find_package
					  IMPORTS 
					  "./res/bindings, imgui_impl_dx11.cpp -> ../imgui/bindings"  
					  "./res/bindings, imgui_impl_dx11.h -> ../imgui/bindings"  
					  "./res/bindings, imgui_impl_win32.cpp -> ../imgui/bindings"  
					  "./res/bindings, imgui_impl_win32.h -> ../imgui/bindings"  
)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
					BUILD missing
					REMOTE conancenter
					SETTINGS ${settings})



#WIL
#FetchContent_Declare(
#  wil
#  GIT_REPOSITORY https://github.com/microsoft/wil.git
#  GIT_TAG origin/master
#  CONFIGURE_COMMAND ""
#  BUILD_COMMAND ""
#)
#FetchContent_GetProperties(wil)
#if(NOT wil_POPULATED)
#  FetchContent_Populate(wil)
#endif()
#
##hbao
#FetchContent_Declare(
#  hbao
#  GIT_REPOSITORY https://github.com/NVIDIAGameWorks/HBAOPlus.git
#  GIT_TAG origin/master
#  CONFIGURE_COMMAND ""
#  BUILD_COMMAND ""
#)
#FetchContent_GetProperties(hbao)
#if(NOT hbao_POPULATED)
#  FetchContent_Populate(hbao)
#endif()



find_program(NUGET_EXE NAMES nuget PATHS ${CMAKE_CURRENT_SOURCE_DIR}/NuGet)
if(NOT NUGET_EXE)
	message("NUGET.EXE not found.")
	message(FATAL_ERROR "Please install this executable, and run CMake again.")
endif()
message(${NUGET_EXE})

exec_program(${NUGET_EXE}
	ARGS install "Microsoft.Windows.CppWinRT" -OutputDirectory ${PACKAGE_DIR})
#exec_program(${NUGET_EXE}
#	ARGS install "DXSDK_Missing" -OutputDirectory ${PACKAGE_DIR})
#exec_program(${NUGET_EXE}
#	ARGS install "directxtex_desktop_win10" -OutputDirectory ${PACKAGE_DIR})



file(GLOB CPPWINRT_DIRS ${PACKAGE_DIR}/Microsoft.Windows.CppWinRT.*)
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

#file(GLOB DXMISS_DIR ${PACKAGE_DIR}/DXSDK_Missing.*)
#file(GLOB DXTEX_DIR ${PACKAGE_DIR}/directxtex_desktop_win10.*)

#set(DXMISSING_HEADERS ${DXMISS_DIR}/Include PARENT_SCOPE)
#set(DXMISSING_LIB ${DXMISS_DIR}/Lib/native/x64/DxErr.lib PARENT_SCOPE)
#set(DXTEX_HEADERS ${DXTEX_DIR}/include PARENT_SCOPE)
#set(DXTEX_LIB ${DXTEX_DIR}/native/lib/x64/$<IF:$<CONFIG:Debug>,Debug,Release>/DirectXTex.lib PARENT_SCOPE)
#set(WIL_HEADERS "${wil_SOURCE_DIR}/include" PARENT_SCOPE)
#set(HBAO_HEADERS "${hbao_SOURCE_DIR}/include" PARENT_SCOPE)
#set(HBAO_LIB "${hbao_SOURCE_DIR}/lib/GFSDK_SSAO_D3D11.win64.lib" PARENT_SCOPE)
#set(HBAO_DYLIB "${hbao_SOURCE_DIR}/lib/GFSDK_SSAO_D3D11.win64.dll" PARENT_SCOPE)
set(CPPWINRT ${CPPWINRT_DIR}/bin/cppwinrt.exe)
set(CPPWINRT_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/winrt)

execute_process(COMMAND
	${CPPWINRT} -input sdk -output ${CPPWINRT_HEADERS}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	RESULT_VARIABLE ret)
if (NOT ret EQUAL 0)
	message(FATAL_ERROR "Failed to run cppwinrt.exe ${CPPWINRT_DIR} ${ret}")
endif()

function(get_winrt var)
	set(var ${CPPWINRT_HEADERS})
endfunction()