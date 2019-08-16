/* v2xctl32.c --- v2xctl32.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetverxp.h"
#include <windows.h>
#include <string.h>
#include <strsafe.h>
#include <psapi.h>

DECLARE_HANDLE(DPI_AWARENESS_CONTEXT);

#define DPI_AWARENESS_CONTEXT_UNAWARE           ((DPI_AWARENESS_CONTEXT)-1)
#define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE      ((DPI_AWARENESS_CONTEXT)-2)
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE ((DPI_AWARENESS_CONTEXT)-3)

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

typedef UINT (WINAPI *FN_GetDpiForWindow)(HWND hwnd);
static FN_GetDpiForWindow s_pGetDpiForWindow = NULL;

typedef DPI_AWARENESS_CONTEXT (WINAPI *FN_SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT);
static FN_SetThreadDpiAwarenessContext s_pSetThreadDpiAwarenessContext = NULL;

BOOL WINAPI
ChangeWindowMessageFilterForXP(UINT message, DWORD dwFlag)
{
    if (s_pChangeWindowMessageFilter && DO_FALLBACK)
        return (*s_pChangeWindowMessageFilter)(message, dwFlag);

    /* otherwise do nothing */
    return TRUE;
}

BOOL WINAPI
ChangeWindowMessageFilterExForXP(HWND hwnd, UINT message, DWORD action,
                                 PCHANGEFILTERSTRUCT pChangeFilterStruct)
{
    if (s_pChangeWindowMessageFilterEx && DO_FALLBACK)
        return (*s_pChangeWindowMessageFilterEx)(hwnd, message, action, pChangeFilterStruct);

    if (s_pChangeWindowMessageFilter && DO_FALLBACK)
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

UINT WINAPI
GetDpiForWindowForXP(HWND hwnd)
{
    if (s_pGetDpiForWindow && DO_FALLBACK)
        return (*s_pGetDpiForWindow)(hwnd);

    if (IsWindow(hwnd))
        return 96;

    return 0;
}

DPI_AWARENESS_CONTEXT WINAPI
SetThreadDpiAwarenessContextForXP(DPI_AWARENESS_CONTEXT dpiContext)
{
    if (s_pSetThreadDpiAwarenessContext && DO_FALLBACK)
        return (*s_pSetThreadDpiAwarenessContext)(dpiContext);

    return DPI_AWARENESS_CONTEXT_UNAWARE;
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
        s_pGetDpiForWindow = (FN_GetDpiForWindow)GetProcAddress(s_hUser32, "GetDpiForWindow");
        s_pSetThreadDpiAwarenessContext = (FN_SetThreadDpiAwarenessContext)GetProcAddress(s_hUser32, "SetThreadDpiAwarenessContext");
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
