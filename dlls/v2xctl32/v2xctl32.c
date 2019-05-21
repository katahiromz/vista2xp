/* v2xctl32.c --- v2xctl32.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetverxp.h"
#include <windows.h>
#include <string.h>
#include <strsafe.h>
#include <psapi.h>
#include "taskdlg.h"

#ifndef ARRAYSIZE
    #define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

static HINSTANCE s_hinstDLL;
static HINSTANCE s_hComCtl32;

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
        StringCchCopyW(psz, cch, psz1);
        StringCchCatW(psz, cch, psz2);
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
    LPWSTR psz0, psz1, pszText;
    WCHAR szTitle[MAX_PATH], szInst[MAX_PATH], szContent[MAX_PATH];

    if (s_pTaskDialog && DO_FALLBACK)
    {
        return (*s_pTaskDialog)(hwndOwner, hInstance, pszWindowTitle, pszMainInstruction,
                                pszContent, dwCommonButtons, pszIcon, pnButton);
    }

    if (!pnButton)
        return E_POINTER;

    *pnButton = 0;

    if (!pszWindowTitle)
    {
        GetModuleFileNameW(NULL, szTitle, ARRAYSIZE(szTitle));
        psz0 = wcsrchr(szTitle, L'\\');
        psz1 = wcsrchr(szTitle, L'/');
        if (!psz0)
            psz0 = psz1;
        if (psz0 < psz1)
            psz0 = psz1;
        pszWindowTitle = psz0 + 1;
    }
    else if (HIWORD(pszWindowTitle) == 0)
    {
        LoadStringW(hInstance, LOWORD(pszWindowTitle), szTitle, ARRAYSIZE(szTitle));
        pszWindowTitle = szTitle;
    }

    if (!pszMainInstruction)
    {
        pszMainInstruction = L"";
    }
    else if (HIWORD(pszMainInstruction) == 0)
    {
        LoadStringW(hInstance, LOWORD(pszMainInstruction), szInst, ARRAYSIZE(szInst));
        pszMainInstruction = szInst;
    }

    if (!pszContent)
    {
        pszContent = L"";
    }
    else if (HIWORD(pszContent) == 0)
    {
        LoadStringW(hInstance, LOWORD(pszContent), szContent, ARRAYSIZE(szContent));
        pszContent = szContent;
    }

    if (!pszMainInstruction || !*pszMainInstruction)
    {
        pszText = _wcsdup(pszContent);
    }
    else
    {
        psz0 = JoinStrings(pszMainInstruction, L"\n\n");
        if (!psz0)
            return E_OUTOFMEMORY;

        pszText = JoinStrings(psz0, pszContent);
        free(psz0);
    }

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
        params.lpszIcon = IDI_HAND;
        params.dwStyle |= MB_USERICON;
        MessageBeep(MB_ICONERROR);
    }
    else if (pszIcon == TD_INFORMATION_ICON)
    {
        params.hInstance = NULL;
        params.lpszIcon = IDI_ASTERISK;
        params.dwStyle |= MB_USERICON;
        MessageBeep(MB_ICONINFORMATION);
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
        params.lpszIcon = IDI_EXCLAMATION;
        params.dwStyle |= MB_USERICON;
        MessageBeep(MB_ICONWARNING);
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
