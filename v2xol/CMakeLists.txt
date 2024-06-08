add_library(v2xol SHARED v2xol.cpp FileDialog.cpp ../ShellItemArray.cpp v2xol.def)
target_link_libraries(v2xol user32 shlwapi comctl32 shell32 ole32)
set_target_properties(v2xol PROPERTIES PREFIX "")
set_target_properties(v2xol PROPERTIES OUTPUT_NAME "v2xol")
