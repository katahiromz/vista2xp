/* v2xctl32.c --- v2xctl32.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetver.h"
#include <windows.h>
#include <string.h>
#include <strsafe.h>
#include <psapi.h>

static HINSTANCE s_hinstDLL;
static HINSTANCE s_hUser32;

#ifndef MSGFLT_ADD
    #define MSGFLT_ADD 1
    #define MSGFLT_REMOVE 2
#endif

typedef BOOL (WINAPI *FN_ChangeWindowMessageFilter)(UINT, DWORD);
static FN_ChangeWindowMessageFilter s_pChangeWindowMessageFilter = NULL;

#ifndef MSGFLTINFO_NONE
    #define MSGFLTINFO_NONE (0)
    #define MSGFLTINFO_ALREADYALLOWED_FORWND (1)
    #define MSGFLTINFO_ALREADYDISALLOWED_FORWND (2)
    #define MSGFLTINFO_ALLOWED_HIGHER (3)

    #define MSGFLT_RESET (0)
    #define MSGFLT_ALLOW (1)
    #define MSGFLT_DISALLOW (2)

    typedef struct tagCHANGEFILTERSTRUCT
    {
        DWORD cbSize;
        DWORD ExtStatus;
    } CHANGEFILTERSTRUCT,*PCHANGEFILTERSTRUCT;
#endif

typedef BOOL (WINAPI *FN_ChangeWindowMessageFilterEx)(HWND, UINT, DWORD, PCHANGEFILTERSTRUCT);
static FN_ChangeWindowMessageFilterEx s_pChangeWindowMessageFilterEx = NULL;

BOOL WINAPI
ChangeWindowMessageFilterForXP(UINT message, DWORD dwFlag)
{
    if (s_pChangeWindowMessageFilter)
        return (*s_pChangeWindowMessageFilter)(message, dwFlag);

    /* otherwise do nothing */
    return TRUE;
}

BOOL WINAPI
ChangeWindowMessageFilterExForXP(HWND hwnd, UINT message, DWORD action,
                                 PCHANGEFILTERSTRUCT pChangeFilterStruct)
{
    if (s_pChangeWindowMessageFilterEx)
        return (*s_pChangeWindowMessageFilterEx)(hwnd, message, action, pChangeFilterStruct);

    if (s_pChangeWindowMessageFilter)
    {
        switch (action)
        {
        case MSGFLT_RESET:
            return (*s_pChangeWindowMessageFilter)(message, MSGFLT_ADD);
        case MSGFLT_ALLOW:
            return (*s_pChangeWindowMessageFilter)(message, MSGFLT_ADD);
        case MSGFLT_DISALLOW:
            return (*s_pChangeWindowMessageFilter)(message, MSGFLT_REMOVE);
        }
    }

    /* otherwise do nothing */
    return TRUE;
}

BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        s_hinstDLL = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
        s_hUser32 = GetModuleHandleA("user32");
        s_pChangeWindowMessageFilter = (FN_ChangeWindowMessageFilter)GetProcAddress(s_hUser32, "ChangeWindowMessageFilter");
        s_pChangeWindowMessageFilterEx = (FN_ChangeWindowMessageFilterEx)GetProcAddress(s_hUser32, "ChangeWindowMessageFilterEx");
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
