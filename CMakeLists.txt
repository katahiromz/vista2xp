# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 9 2008" .
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles" .
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 3.6)

# project name and languages
project(vista2xp C CXX RC)

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

# libWonFinalPathName.a
add_subdirectory(WonFinalPathName)

# vista2xp.exe
add_subdirectory(vista2xp)

# v2xker32.dll
add_subdirectory(v2xker32)

# v2xu32.dll
add_subdirectory(v2xu32)

# v2xctl32.dll
add_subdirectory(v2xctl32)

# v2xol.dll
add_subdirectory(v2xol)

# v2xsh32.dll
add_subdirectory(v2xsh32)

# v2xcrt.dll
add_subdirectory(v2xcrt)

# v2xadv32.dll
add_subdirectory(v2xadv32)

# dll files and test programs
add_subdirectory(tests)

##############################################################################
