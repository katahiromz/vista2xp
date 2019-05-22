/* localized.c --- vista2xp testcase
 * This file is public domain software.
 * Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
 */

#include "targetvervista.h"
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
#if defined(__GNUC__) && (__GNUC__ < 7)
    printf("SHGetLocalizedName is not supported\n");
#else
    INT ac = 0;
    LPWSTR *av = CommandLineToArgvW(GetCommandLineW(), &ac);
    WCHAR szPath[MAX_PATH];
    INT ids;
    for (INT i = 1; i < ac; ++i)
    {
        lstrcpyW(szPath, L"(invalid)");
        ids = 0xDEADBEEF;
        HRESULT hr = SHGetLocalizedName(av[i], szPath, MAX_PATH, &ids);
        printf("hr: %08X\n", hr);
        printf("original: '%ls'\n", av[i]);
        printf("localized: '%ls'\n", szPath);
        printf("ids: %08X\n", ids);
    }
#endif
    return 0;
}
