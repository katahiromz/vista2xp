# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 9 2008" .
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles" .
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 3.6)

# project name and languages
project(vista2xp C CXX)

# check build type
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# make uppercase string of build type
string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)

# check build type again
if (NOT uppercase_CMAKE_BUILD_TYPE MATCHES "^(DEBUG|RELEASE|RELWITHDEBINFO|MINSIZEREL)$")
    message(FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

# set output directory (build/)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if (WIN32)
    # enable Win32 resource
    enable_language(RC)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # using Clang
        set(CMAKE_C_FLAGS "-static -s")
        set(CMAKE_CXX_FLAGS "-static -s")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # using GCC
        set(CMAKE_C_FLAGS "-static -static-libgcc -static-libstdc++ -s")
        set(CMAKE_CXX_FLAGS "-static -static-libgcc -static-libstdc++ -s")
    elseif (MSVC)
        # replace "/MD" with "/MT" (building without runtime DLLs)
        set(CompilerFlags
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        foreach(CompilerFlags ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlags} "${${CompilerFlags}}")
        endforeach()
    endif()
endif()

##############################################################################

option(DO_FALLBACK "Do fallback" ON)
if (DO_FALLBACK)
    add_definitions(-DDO_FALLBACK=1)
    message(STATUS DO_FALLBACK=1)
else()
    add_definitions(-DDO_FALLBACK=0)
    message(STATUS DO_FALLBACK=0)
endif()

# use targetverxp.h and targetvervista.h
include_directories(include)

# support TDM-GCC-32
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    execute_process(COMMAND gcc -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    message(STATUS "GCC_VERSION = ${GCC_VERSION}")
    if ("${GCC_VERSION}" STRLESS "7.0.0")
        include_directories(tdm_gcc_include)
        add_definitions(-DSHParseDisplayName_WORKAROUND)
    endif()
endif()

# Unicode support
add_definitions(-DUNICODE -D_UNICODE)

# libWonFileID.a
add_subdirectory(WonFileID)
include_directories(WonFileID)

# libWonFinalPathName.a
add_subdirectory(WonFinalPathName)
include_directories(WonFinalPathName)

# vista2xp.exe
add_executable(vista2xp WIN32
    vista2xp/vista2xp.cpp
    vista2xp/vista2xp_res.rc
    vista2xp/JustDoIt.cpp)
target_link_libraries(vista2xp comdlg32 comctl32 shlwapi)

# dll files and test programs
add_subdirectory(tests)

##############################################################################

include_directories(AFTER .)

# v2xker32.dll
add_library(v2xker32 SHARED v2xker32/v2xker32.c v2xker32/v2xker32.def)
target_link_libraries(v2xker32 kernel32 advapi32 winmm psapi WonFileID WonFinalPathName)
set_target_properties(v2xker32 PROPERTIES PREFIX "")
set_target_properties(v2xker32 PROPERTIES OUTPUT_NAME "v2xker32")

# v2xu32.dll
add_library(v2xu32 SHARED v2xu32/v2xu32.c v2xu32/v2xu32.def)
target_link_libraries(v2xu32 user32)
set_target_properties(v2xu32 PROPERTIES PREFIX "")
set_target_properties(v2xu32 PROPERTIES OUTPUT_NAME "v2xu32")

# v2xctl32.dll
add_library(v2xctl32 SHARED v2xctl32/v2xctl32.c v2xctl32/v2xctl32.def v2xctl32/v2xctl32_res.rc)
target_link_libraries(v2xctl32 user32 comctl32)
set_target_properties(v2xctl32 PROPERTIES PREFIX "")
set_target_properties(v2xctl32 PROPERTIES OUTPUT_NAME "v2xctl32")

# v2xol.dll
add_library(v2xol SHARED v2xol/v2xol.cpp v2xol/FileDialog.cpp ShellItemArray.cpp v2xol/v2xol.def)
target_link_libraries(v2xol user32 shlwapi comctl32 shell32 ole32)
set_target_properties(v2xol PROPERTIES PREFIX "")
set_target_properties(v2xol PROPERTIES OUTPUT_NAME "v2xol")

# v2xsh32.dll
add_library(v2xsh32 SHARED v2xsh32/v2xsh32.cpp ShellItemArray.cpp v2xsh32/v2xsh32.def)
target_link_libraries(v2xsh32 user32 shlwapi comctl32 shell32 ole32)
set_target_properties(v2xsh32 PROPERTIES PREFIX "")
set_target_properties(v2xsh32 PROPERTIES OUTPUT_NAME "v2xsh32")

# v2xcrt.dll
add_library(v2xcrt SHARED v2xcrt/v2xcrt.c v2xcrt/v2xcrt.def)
target_link_libraries(v2xcrt user32)
set_target_properties(v2xcrt PROPERTIES PREFIX "")
set_target_properties(v2xcrt PROPERTIES OUTPUT_NAME "v2xcrt")

# v2xadv32.dll
add_library(v2xadv32 SHARED v2xadv32/v2xadv32.c v2xadv32/v2xadv32.def)
target_link_libraries(v2xadv32 kernel32 advapi32)
set_target_properties(v2xadv32 PROPERTIES PREFIX "")
set_target_properties(v2xadv32 PROPERTIES OUTPUT_NAME "v2xadv32")

##############################################################################
