# taskdlg.exe
add_executable(taskdlg taskdlg/taskdlg.c taskdlg/taskdlg_res.rc)
target_link_libraries(taskdlg comctl32)

# taskdlgi.exe
add_executable(taskdlgi taskdlgi/taskdlgi.c taskdlgi/taskdlgi_res.rc)
target_link_libraries(taskdlgi comctl32)

# regtest.exe
add_executable(regtest regtest/regtest.c)

# opendlg.exe
add_executable(opendlg opendlg/opendlg.cpp)
target_link_libraries(opendlg uuid)

# savedlg.exe
add_executable(savedlg savedlg/savedlg.cpp)
target_link_libraries(savedlg uuid)

# multifile.exe
add_executable(multifile multifile/multifile.cpp)
target_link_libraries(multifile uuid)

# folder.exe
add_executable(folder folder/folder.cpp)
target_link_libraries(folder uuid)

# localized.exe
add_executable(localized localized/localized.cpp)
target_link_libraries(localized shell32)

# winverex.exe
add_executable(winverex WIN32 winverex/winverex.c)
