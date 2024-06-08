add_library(v2xker32 SHARED v2xker32.c v2xker32.def)
target_link_libraries(v2xker32 kernel32 advapi32 winmm psapi WonFileID WonFinalPathName shlwapi)
set_target_properties(v2xker32 PROPERTIES PREFIX "")
set_target_properties(v2xker32 PROPERTIES OUTPUT_NAME "v2xker32")
