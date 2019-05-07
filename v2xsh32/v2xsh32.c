/* v2xsh32.c --- v2xsh32.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetver.h"
#include <windows.h>
#include <objbase.h>
#include <strsafe.h>

static HINSTANCE s_hinstDLL;
static HINSTANCE s_hShell32;

BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        s_hinstDLL = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
        s_hShell32 = GetModuleHandleA("shell32");
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
