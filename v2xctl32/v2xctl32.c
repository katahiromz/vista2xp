/* v2xctl32.c --- v2xctl32.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetver.h"
#include <windows.h>
#include <string.h>
#include <strsafe.h>
#include <psapi.h>

static HINSTANCE s_hinstDLL;
static HINSTANCE s_hComCtl32;

#ifndef TD_WARNING_ICON
    #define TD_WARNING_ICON MAKEINTRESOURCEW (-1)
    #define TD_ERROR_ICON MAKEINTRESOURCEW (-2)
    #define TD_INFORMATION_ICON MAKEINTRESOURCEW (-3)
    #define TD_SHIELD_ICON MAKEINTRESOURCEW (-4)
    enum _TASKDIALOG_COMMON_BUTTON_FLAGS
    {
        TDCBF_OK_BUTTON = 0x1,
        TDCBF_YES_BUTTON = 0x2,
        TDCBF_NO_BUTTON = 0x4,
        TDCBF_CANCEL_BUTTON = 0x8,
        TDCBF_RETRY_BUTTON = 0x10,
        TDCBF_CLOSE_BUTTON = 0x20
    };
    typedef int TASKDIALOG_COMMON_BUTTON_FLAGS;
#endif

// TaskDialog
typedef HRESULT (WINAPI *FN_TaskDialog)(HWND, HINSTANCE, PCWSTR, PCWSTR, PCWSTR, TASKDIALOG_COMMON_BUTTON_FLAGS, PCWSTR, int *);
static FN_TaskDialog s_pTaskDialog = NULL;

LPWSTR JoinStrings(LPCWSTR psz1, LPCWSTR psz2)
{
    INT cch, cch1 = lstrlenW(psz1), cch2 = lstrlenW(psz2);
    LPWSTR psz;

    cch = cch1 + 1 + cch2 + 1;
    psz = (LPWSTR)malloc(cch * sizeof(WCHAR));
    if (psz)
    {
        StringCbCopyW(psz, cch, psz1);
        StringCbCatW(psz, cch, psz2);
    }
    return psz;
}

static DWORD StyleFromCommonButtons(DWORD dwCommonButtons)
{
    DWORD dwStyle = MB_USERICON;

    if ((dwCommonButtons & TDCBF_OK_BUTTON) && (dwCommonButtons & TDCBF_CANCEL_BUTTON))
        return dwStyle | MB_OKCANCEL;

    if ((dwCommonButtons & TDCBF_RETRY_BUTTON) && (dwCommonButtons & TDCBF_CANCEL_BUTTON))
        return dwStyle | MB_RETRYCANCEL;

    if ((dwCommonButtons & TDCBF_YES_BUTTON) &&
        (dwCommonButtons & TDCBF_NO_BUTTON) &&
        (dwCommonButtons & TDCBF_CANCEL_BUTTON))
    {
        return dwStyle | MB_YESNOCANCEL;
    }

    if ((dwCommonButtons & TDCBF_YES_BUTTON) &&
        (dwCommonButtons & TDCBF_NO_BUTTON))
    {
        return dwStyle | MB_YESNO;
    }

    return dwStyle;
}

HRESULT WINAPI
TaskDialogForXP(HWND hwndOwner, HINSTANCE hInstance, PCWSTR pszWindowTitle,
                PCWSTR pszMainInstruction, PCWSTR pszContent,
                TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons,
                PCWSTR pszIcon, int *pnButton)
{
    MSGBOXPARAMSW params;
    LPWSTR psz0, pszText;

    if (s_pTaskDialog && 0)
    {
        return (*s_pTaskDialog)(hwndOwner, hInstance, pszWindowTitle, pszMainInstruction,
                                pszContent, dwCommonButtons, pszIcon, pnButton);
    }

    if (!pnButton)
        return E_POINTER;

    *pnButton = 0;

    psz0 = JoinStrings(pszMainInstruction, L"\n\n");
    if (!psz0)
        return E_OUTOFMEMORY;

    pszText = JoinStrings(psz0, pszContent);
    free(psz0);

    if (!pszText)
        return E_OUTOFMEMORY;

    ZeroMemory(&params, sizeof(params));
    params.cbSize = sizeof(params);
    params.hwndOwner = hwndOwner;
    params.lpszText = pszText;
    params.lpszCaption = pszWindowTitle;
    params.dwStyle = StyleFromCommonButtons(dwCommonButtons);

    if (pszIcon == TD_ERROR_ICON)
    {
        params.hInstance = NULL;
        params.lpszIcon = IDI_EXCLAMATION;
        params.dwStyle |= MB_USERICON;
    }
    else if (pszIcon == TD_INFORMATION_ICON)
    {
        params.hInstance = NULL;
        params.lpszIcon = IDI_ASTERISK;
        params.dwStyle |= MB_USERICON;
    }
    else if (pszIcon == TD_SHIELD_ICON)
    {
        params.hInstance = s_hinstDLL;
        params.lpszIcon = MAKEINTRESOURCEW(100);
        params.dwStyle |= MB_USERICON;
    }
    else if (pszIcon == TD_WARNING_ICON)
    {
        params.hInstance = NULL;
        params.lpszIcon = IDI_HAND;
        params.dwStyle |= MB_USERICON;
    }
    else
    {
        if (hInstance == NULL)
        {
            free(pszText);
            return E_FAIL;
        }

        params.hInstance = hInstance;
        params.lpszIcon = pszIcon;
        params.dwStyle |= MB_USERICON;
    }

    *pnButton = MessageBoxIndirectW(&params);
    free(pszText);

    return S_OK;
}

BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        s_hinstDLL = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
        s_hComCtl32 = GetModuleHandleA("comctl32");
        s_pTaskDialog = (FN_TaskDialog)GetProcAddress(s_hComCtl32, "TaskDialog");
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
