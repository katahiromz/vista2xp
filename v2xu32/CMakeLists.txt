add_library(v2xu32 SHARED v2xu32.c v2xu32.def)
target_link_libraries(v2xu32 user32)
set_target_properties(v2xu32 PROPERTIES PREFIX "")
set_target_properties(v2xu32 PROPERTIES OUTPUT_NAME "v2xu32")
