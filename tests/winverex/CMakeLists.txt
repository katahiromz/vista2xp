# winverex.exe
add_executable(winverex WIN32 winverex.c winverex_res.rc)
target_link_libraries(winverex PRIVATE comctl32)
