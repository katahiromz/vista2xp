add_library(v2xcrt SHARED v2xcrt.c v2xcrt.def)
target_link_libraries(v2xcrt user32)
set_target_properties(v2xcrt PROPERTIES PREFIX "")
set_target_properties(v2xcrt PROPERTIES OUTPUT_NAME "v2xcrt")
