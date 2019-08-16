/* v2xcrt.c --- v2xcrt.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetverxp.h"
#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <strsafe.h>

#ifndef ARRAYSIZE
    #define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

static HINSTANCE s_hinstDLL;
static HINSTANCE s_hMSVCRT;

// wcsnlen
typedef size_t (__cdecl *FN_wcsnlen)(const wchar_t *, size_t);
static FN_wcsnlen s_pwcsnlen = NULL;

size_t __cdecl wcsnlen_forxp(const wchar_t *s, size_t maxlen)
{
    size_t i;

    if (s_pwcsnlen && DO_FALLBACK)
    {
        return (*s_pwcsnlen)(s, maxlen);
    }

    for (i = 0; s[i] && i < maxlen; ++i)
    {
        ;
    }

    return i;
}

#define GETPROC(fn) s_p##fn = (FN_##fn)GetProcAddress(s_hMSVCRT, #fn)

BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        s_hinstDLL = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
        s_hMSVCRT = GetModuleHandleA("msvcrt");
        GETPROC(wcsnlen);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
