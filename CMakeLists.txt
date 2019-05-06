# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 9 2008" .
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles" .
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 2.4)

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

# use targetver.h
include_directories(.)

# Unicode support
add_definitions(-DUNICODE -D_UNICODE)

# v2xker32.dll
add_library(v2xker32 SHARED v2xker32/v2xker32.c v2xker32/v2xker32.def)
target_link_libraries(v2xker32 kernel32 advapi32 winmm psapi)
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

# vista2xp.exe
add_executable(vista2xp WIN32 vista2xp/vista2xp.c vista2xp/vista2xp_res.rc vista2xp/JustDoIt.cpp)
target_link_libraries(vista2xp comdlg32 comctl32 shlwapi)

##############################################################################
