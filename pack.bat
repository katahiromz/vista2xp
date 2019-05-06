set DIST=vista2xp-0.4
if not exist %DIST% mkdir %DIST%
copy README.txt %DIST%
copy READMEJP.txt %DIST%
copy LICENSE.txt %DIST%
copy build\vista2xp.exe %DIST%
copy build\v2xker32.dll %DIST%
copy build\v2xctl32.dll %DIST%
copy build\v2xu32.dll %DIST%
