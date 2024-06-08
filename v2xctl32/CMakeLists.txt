add_library(v2xctl32 SHARED v2xctl32.c v2xctl32.def v2xctl32_res.rc)
target_link_libraries(v2xctl32 user32 comctl32)
set_target_properties(v2xctl32 PROPERTIES PREFIX "")
set_target_properties(v2xctl32 PROPERTIES OUTPUT_NAME "v2xctl32")
