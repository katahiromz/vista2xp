/* winverex.c --- vista2xp testcase
 * This file is public domain software.
 * Copyright (C) 2024 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
 */

#include "targetvervista.h"
#include <windows.h>

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    DWORD dwVersion = GetVersion();
    OSVERSIONINFOA osver = { sizeof(osver) };
    GetVersionExA(&osver);
    CHAR szText[64];
    CHAR szText2[1024];
    szText2[0] = 0;
    wsprintfA(szText, "GetVersion: 0x%08lX\n", dwVersion);
    lstrcatA(szText2, szText);
    wsprintfA(szText, "osver.dwMajorVersion: %ld\n", osver.dwMajorVersion);
    lstrcatA(szText2, szText);
    wsprintfA(szText, "osver.dwMinorVersion: %ld\n", osver.dwMinorVersion);
    lstrcatA(szText2, szText);
    wsprintfA(szText, "osver.dwBuildNumber: %ld\n", osver.dwBuildNumber);
    lstrcatA(szText2, szText);
    wsprintfA(szText, "osver.dwPlatformId: %ld\n", osver.dwPlatformId);
    lstrcatA(szText2, szText);
    wsprintfA(szText, "osver.szCSDVersion: %s\n", osver.szCSDVersion);
    lstrcatA(szText2, szText);
    MessageBoxA(NULL, szText2, "INFO", 0);
}
