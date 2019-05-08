include_directories(.)

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

# v2xol.dll
add_library(v2xol SHARED v2xol/v2xol.cpp v2xol/FileDialog.cpp ShellItemArray.cpp v2xol/v2xol.def)
target_link_libraries(v2xol user32 shlwapi comctl32 shell32)
set_target_properties(v2xol PROPERTIES PREFIX "")
set_target_properties(v2xol PROPERTIES OUTPUT_NAME "v2xol")
