# vista2xp.exe
add_executable(vista2xp WIN32
    vista2xp.cpp
    vista2xp_res.rc
    JustDoIt.cpp)
target_link_libraries(vista2xp comdlg32 comctl32 shlwapi)
