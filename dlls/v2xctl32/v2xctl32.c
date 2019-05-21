/* v2xctl32.c --- v2xctl32.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetverxp.h"
#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <strsafe.h>
#include <psapi.h>
#include "taskdlg.h"
#include "resource.h"

#ifndef ARRAYSIZE
    #define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

static HINSTANCE s_hinstDLL;
static HINSTANCE s_hComCtl32;

// TaskDialog
typedef HRESULT (WINAPI *FN_TaskDialog)(HWND, HINSTANCE, PCWSTR, PCWSTR, PCWSTR, TASKDIALOG_COMMON_BUTTON_FLAGS, PCWSTR, int *);
// TaskDialogIndirect
typedef HRESULT (WINAPI *FN_TaskDialogIndirect)(const TASKDIALOGCONFIG *, int *, int *, BOOL *);

static FN_TaskDialog s_pTaskDialog = NULL;
static FN_TaskDialogIndirect s_pTaskDialogIndirect = NULL;

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
        params.lpszIcon = MAKEINTRESOURCEW(IDI_SHIELD_ICON);
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

static BOOL TaskDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    const TASKDIALOGCONFIG *pTaskConfig = (const TASKDIALOGCONFIG *)lParam;
    HINSTANCE hInstance = pTaskConfig->hInstance;
    PCWSTR pszWindowTitle = pTaskConfig->pszWindowTitle;
    PCWSTR pszMainInstruction = pTaskConfig->pszMainInstruction;
    PCWSTR pszContent = pTaskConfig->pszContent;
    TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons = pTaskConfig->dwCommonButtons;
    PCWSTR pszIcon = pTaskConfig->pszMainIcon;
    LPWSTR psz0, psz1, pszText, pszButton, pch, pszFooter;
    WCHAR szTitle[MAX_PATH], szInst[MAX_PATH], szContent[MAX_PATH], szButtonText[64], szFooter[MAX_PATH];
    INT i, id, cyCommandLink, cyButtons;
    RECT rc1, rc2;
    HWND hCtrl;
    HDC hDC;
    HFONT hFont;
    HGDIOBJ hFontOld;
    POINT pt;
    SIZE siz;
    HWND hStc2;

    SetWindowLongPtr(hwnd, DWLP_USER, lParam);

    // title
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
    SetWindowText(hwnd, pszWindowTitle);

    // main instruction
    if (!pszMainInstruction)
    {
        pszMainInstruction = L"";
    }
    else if (HIWORD(pszMainInstruction) == 0)
    {
        LoadStringW(hInstance, LOWORD(pszMainInstruction), szInst, ARRAYSIZE(szInst));
        pszMainInstruction = szInst;
    }

    // context
    if (!pszContent)
    {
        pszContent = L"";
    }
    else if (HIWORD(pszContent) == 0)
    {
        LoadStringW(hInstance, LOWORD(pszContent), szContent, ARRAYSIZE(szContent));
        pszContent = szContent;
    }

    // build pszText
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
    {
        EndDialog(hwnd, IDCLOSE);
        return FALSE;
    }

    SetDlgItemText(hwnd, stc1, pszText);

    // icon
    if (pTaskConfig->dwFlags & TDF_USE_HICON_MAIN)
    {
        SendDlgItemMessage(hwnd, ico1, STM_SETICON, (WPARAM)pTaskConfig->hMainIcon, 0);
    }
    else if (pszIcon == TD_ERROR_ICON)
    {
        HICON hIcon = LoadIcon(NULL, IDI_HAND);
        SendDlgItemMessage(hwnd, ico1, STM_SETICON, (WPARAM)hIcon, 0);
        MessageBeep(MB_ICONERROR);
    }
    else if (pszIcon == TD_INFORMATION_ICON)
    {
        HICON hIcon = LoadIcon(NULL, IDI_ASTERISK);
        SendDlgItemMessage(hwnd, ico1, STM_SETICON, (WPARAM)hIcon, 0);
        MessageBeep(MB_ICONINFORMATION);
    }
    else if (pszIcon == TD_SHIELD_ICON)
    {
        HICON hIcon = LoadIcon(s_hinstDLL, MAKEINTRESOURCEW(IDI_SHIELD_ICON));
        SendDlgItemMessage(hwnd, ico1, STM_SETICON, (WPARAM)hIcon, 0);
    }
    else if (pszIcon == TD_WARNING_ICON)
    {
        HICON hIcon = LoadIcon(NULL, IDI_EXCLAMATION);
        SendDlgItemMessage(hwnd, ico1, STM_SETICON, (WPARAM)hIcon, 0);
        MessageBeep(MB_ICONWARNING);
    }
    else
    {
        if (hInstance == NULL)
        {
            free(pszText);
            EndDialog(hwnd, IDCLOSE);
            return FALSE;
        }
        else
        {
            HICON hIcon = LoadIcon(hInstance, pszIcon);
            SendDlgItemMessage(hwnd, ico1, STM_SETICON, (WPARAM)hIcon, 0);
        }
    }

    // calculate cyCommandLink
    GetWindowRect(GetDlgItem(hwnd, psh1), &rc1);
    GetWindowRect(GetDlgItem(hwnd, psh2), &rc2);
    pt.x = rc1.left;
    pt.y = rc1.top;
    MapWindowPoints(NULL, hwnd, &pt, 1);
    cyCommandLink = rc2.top - rc1.top;

    GetWindowRect(GetDlgItem(hwnd, psh7), &rc1);
    cyButtons = rc1.bottom - rc1.top;

    // shrink dialog box
    GetWindowRect(hwnd, &rc1);
    if (pTaskConfig->dwFlags & TDF_USE_COMMAND_LINKS)
    {
        rc1.bottom -= (6 - pTaskConfig->cButtons) * cyCommandLink;
        rc1.bottom -= cyButtons;

        DestroyWindow(GetDlgItem(hwnd, psh7));
        DestroyWindow(GetDlgItem(hwnd, psh8));
        DestroyWindow(GetDlgItem(hwnd, psh9));
        DestroyWindow(GetDlgItem(hwnd, psh10));
        DestroyWindow(GetDlgItem(hwnd, psh11));
        DestroyWindow(GetDlgItem(hwnd, psh12));

        for (i = 0; i < 6; ++i)
        {
            hCtrl = GetDlgItem(hwnd, psh1 + i);

            if (i >= pTaskConfig->cButtons)
            {
                DestroyWindow(hCtrl);
                continue;
            }

            // set text
            pszButton = (LPWSTR)pTaskConfig->pButtons[i].pszButtonText;
            if (HIWORD(pszButton) == 0)
            {
                LoadString(hInstance, LOWORD(pszButton), szButtonText, ARRAYSIZE(szButtonText));
                pszButton = szButtonText;
            }
            pszButton = _wcsdup(pszButton);
            pch = wcschr(pszButton, L'\n');
            if (pch)
                *pch = 0;
            SetWindowText(hCtrl, pszButton);
            free(pszButton);

            // set id
            id = pTaskConfig->pButtons[i].nButtonID;
            SetWindowLongPtr(hCtrl, GWLP_ID, id);
        }
    }
    else
    {
        rc1.bottom -= 6 * cyCommandLink;

        DestroyWindow(GetDlgItem(hwnd, psh1));
        DestroyWindow(GetDlgItem(hwnd, psh2));
        DestroyWindow(GetDlgItem(hwnd, psh3));
        DestroyWindow(GetDlgItem(hwnd, psh4));
        DestroyWindow(GetDlgItem(hwnd, psh5));
        DestroyWindow(GetDlgItem(hwnd, psh6));

        for (i = 0; i < 6; ++i)
        {
            hCtrl = GetDlgItem(hwnd, psh7 + i);

            if (i >= pTaskConfig->cButtons)
            {
                DestroyWindow(hCtrl);
                continue;
            }

            hDC = GetDC(hCtrl);
            hFont = GetWindowFont(hCtrl);
            hFontOld = SelectObject(hDC, hFont);

            // set text
            pszButton = (LPWSTR)pTaskConfig->pButtons[i].pszButtonText;
            if (HIWORD(pszButton) == 0)
            {
                LoadString(hInstance, LOWORD(pszButton), szButtonText, ARRAYSIZE(szButtonText));
                pszButton = szButtonText;
            }
            pszButton = _wcsdup(pszButton);
            pch = wcschr(pszButton, L'\n');
            if (pch)
                *pch = 0;
            SetWindowText(hCtrl, pszButton);
            GetTextExtentPoint32(hDC, pszButton, lstrlen(pszButton), &siz);
            free(pszButton);

            SelectObject(hDC, hFontOld);

            // move
            GetWindowRect(hCtrl, &rc2);
            MapWindowRect(NULL, hwnd, &rc2);
            MoveWindow(hCtrl,
                pt.x, rc2.top - cyCommandLink * 6,
                siz.cx + 16,
                rc2.bottom - rc2.top,
                TRUE);

            // set id
            id = pTaskConfig->pButtons[i].nButtonID;
            SetWindowLongPtr(hCtrl, GWLP_ID, id);

            ReleaseDC(hCtrl, hDC);

            pt.x += siz.cx + 24;
        }
    }

    // footer
    hStc2 = GetDlgItem(hwnd, stc2);
    GetWindowRect(hStc2, &rc2);
    MapWindowRect(NULL, hwnd, &rc2);
    pszFooter = (LPWSTR)pTaskConfig->pszFooter;
    if (pszFooter && pszFooter[0])
    {
        if (HIWORD(pszFooter) == 0)
        {
            LoadString(hInstance, LOWORD(pszFooter), szFooter, ARRAYSIZE(szFooter));
            pszFooter = szFooter;
        }
        SetWindowText(hStc2, pszFooter);

        if (pTaskConfig->dwFlags & TDF_USE_COMMAND_LINKS)
        {
            MoveWindow(hStc2,
                rc2.left, rc2.top - cyButtons,
                rc2.right - rc2.left, rc2.bottom - rc2.top,
                TRUE);
        }
        else
        {
            MoveWindow(hStc2,
                rc2.left, rc2.top - cyCommandLink * 6,
                rc2.right - rc2.left, rc2.bottom - rc2.top,
                TRUE);
        }
    }
    else
    {
        rc1.bottom -= rc2.bottom - rc2.top;
        DestroyWindow(hStc2);
    }

    MoveWindow(hwnd, rc1.left, rc1.top, rc1.right - rc1.left, rc1.bottom - rc1.top, TRUE);

    if (!(pTaskConfig->dwFlags & TDF_ALLOW_DIALOG_CANCELLATION))
    {
        HMENU hMenu = GetSystemMenu(hwnd, FALSE);
        DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
    }

    if (pTaskConfig->nDefaultButton)
    {
        SendMessage(hwnd, DM_SETDEFID, pTaskConfig->nDefaultButton, 0);
        SetFocus(GetDlgItem(hwnd, pTaskConfig->nDefaultButton));
        return FALSE;
    }

    return TRUE;
}

static void TaskDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    const TASKDIALOGCONFIG *pTaskConfig;
    pTaskConfig = (const TASKDIALOGCONFIG *)GetWindowLongPtr(hwnd, DWLP_USER);

    if (codeNotify == BN_CLICKED)
        EndDialog(hwnd, id);

    if (pTaskConfig->dwFlags & TDF_ALLOW_DIALOG_CANCELLATION)
    {
        if (id == IDCANCEL)
            EndDialog(hwnd, IDCANCEL);
    }
}

static INT_PTR CALLBACK
TaskDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, TaskDlg_OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, TaskDlg_OnCommand);
    case WM_CTLCOLORSTATIC:
        if ((HWND)lParam == GetDlgItem(hwnd, stc1) ||
            (HWND)lParam == GetDlgItem(hwnd, stc2))
        {
            return SetDlgMsgResult(hwnd, WM_CTLCOLORSTATIC, GetStockBrush(WHITE_BRUSH));
        }
        break;
    }
    return 0;
}

HRESULT WINAPI
TaskDialogIndirectForXP(const TASKDIALOGCONFIG *pTaskConfig,
                        int *pnButton, int *pnRadioButton,
                        BOOL *pfVerificationFlagChecked)
{
    if (s_pTaskDialogIndirect && DO_FALLBACK)
    {
        return (*s_pTaskDialogIndirect)(pTaskConfig, pnButton, pnRadioButton,
                                        pfVerificationFlagChecked);
    }

    if (!pTaskConfig || pTaskConfig->cbSize != sizeof(*pTaskConfig))
    {
        return E_INVALIDARG;
    }

    if (!pTaskConfig->cButtons || !pTaskConfig->pButtons)
    {
        return TaskDialogForXP(pTaskConfig->hwndParent, pTaskConfig->hInstance,
                               pTaskConfig->pszWindowTitle,
                               pTaskConfig->pszMainInstruction,
                               pTaskConfig->pszContent,
                               pTaskConfig->dwCommonButtons,
                               pTaskConfig->pszMainIcon, pnButton);
    }

    if (pnButton)
    {
        *pnButton = DialogBoxParamW(s_hinstDLL, MAKEINTRESOURCEW(IDD_TASKDLG),
                                    pTaskConfig->hwndParent, TaskDlgProc, (LPARAM)pTaskConfig);
    }
    else
    {
        DialogBoxParamW(s_hinstDLL, MAKEINTRESOURCEW(IDD_TASKDLG),
                        pTaskConfig->hwndParent, TaskDlgProc, (LPARAM)pTaskConfig);
    }

    return S_OK;
}

#define GETPROC(fn) s_p##fn = (FN_##fn)GetProcAddress(s_hComCtl32, #fn)

BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        s_hinstDLL = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
        s_hComCtl32 = GetModuleHandleA("comctl32");
        GETPROC(TaskDialog);
        GETPROC(TaskDialogIndirect);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
