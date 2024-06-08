/* winverex.c --- vista2xp testcase
 * This file is public domain software.
 * Copyright (C) 2024 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
 */

#include "targetvervista.h"
#include <windows.h>
#include <stdio.h>

int main(void)
{
    DWORD dwVersion = GetVersion();
    OSVERSIONINFO osver = { sizeof(osver) };
    GetVersionEx(&osver);
    printf("GetVersion: 0x%08lX\n", dwVersion);
    printf("osver.dwMajorVersion: %ld\n", osver.dwMajorVersion);
    printf("osver.dwMinorVersion: %ld\n", osver.dwMinorVersion);
    printf("osver.dwBuildNumber: %ld\n", osver.dwBuildNumber);
    printf("osver.dwPlatformId: %ld\n", osver.dwPlatformId);
}
