# taskdlg.exe
add_executable(taskdlg taskdlg/taskdlg.c taskdlg/taskdlg_res.rc)
target_link_libraries(taskdlg comctl32)

# regtest.exe
add_executable(regtest regtest/regtest.c)

# opendlg.exe
add_executable(opendlg opendlg/opendlg.cpp)
target_link_libraries(opendlg uuid)

# savedlg.exe
add_executable(savedlg savedlg/savedlg.cpp)
target_link_libraries(savedlg uuid)
