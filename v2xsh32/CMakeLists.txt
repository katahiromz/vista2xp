add_library(v2xsh32 SHARED v2xsh32.cpp ../ShellItemArray.cpp v2xsh32.def)
target_link_libraries(v2xsh32 user32 shlwapi comctl32 shell32 ole32)
set_target_properties(v2xsh32 PROPERTIES PREFIX "")
set_target_properties(v2xsh32 PROPERTIES OUTPUT_NAME "v2xsh32")
