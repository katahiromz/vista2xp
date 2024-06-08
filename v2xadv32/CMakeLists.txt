add_library(v2xadv32 SHARED v2xadv32.c v2xadv32.def)
target_link_libraries(v2xadv32 kernel32 advapi32)
set_target_properties(v2xadv32 PROPERTIES PREFIX "")
set_target_properties(v2xadv32 PROPERTIES OUTPUT_NAME "v2xadv32")
