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

VOID DoRepositionPointDx(LPPOINT ppt, SIZE siz, LPCRECT prc)
{
    if (ppt->x + siz.cx > prc->right)
        ppt->x = prc->right - siz.cx;
    if (ppt->y + siz.cy > prc->bottom)
        ppt->y = prc->bottom - siz.cy;
    if (ppt->x < prc->left)
        ppt->x = prc->left;
    if (ppt->y < prc->top)
        ppt->y = prc->top;
}

RECT WorkAreaFromWindowDx(HWND hwnd)
{
    RECT rc;
#if (WINVER >= 0x0500)
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (GetMonitorInfo(hMonitor, &mi))
    {
        return mi.rcWork;
    }
#endif
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
    return rc;
}

void DoCenterWindow(HWND hwnd)
{
    HWND hwndParent;
    BOOL bChild = !!(GetWindowStyle(hwnd) & WS_CHILD);
    RECT rcWorkArea, rcParent;
    SIZE sizParent, siz;
    RECT rc;
    POINT pt;

    if (bChild)
        hwndParent = GetParent(hwnd);
    else
        hwndParent = GetWindow(hwnd, GW_OWNER);

    rcWorkArea = WorkAreaFromWindowDx(hwnd);

    if (hwndParent)
        GetWindowRect(hwndParent, &rcParent);
    else
        rcParent = rcWorkArea;

    sizParent.cx = rcParent.right - rcParent.left;
    sizParent.cy = rcParent.bottom - rcParent.top;

    GetWindowRect(hwnd, &rc);
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    pt.x = rcParent.left + (sizParent.cx - siz.cx) / 2;
    pt.y = rcParent.top + (sizParent.cy - siz.cy) / 2;

    if (bChild && hwndParent)
    {
        GetClientRect(hwndParent, &rcParent);
        MapWindowPoints(hwndParent, NULL, (LPPOINT)&rcParent, 2);
        DoRepositionPointDx(&pt, siz, &rcParent);

        ScreenToClient(hwndParent, &pt);
    }
    else
    {
        DoRepositionPointDx(&pt, siz, &rcWorkArea);
    }

    SetWindowPos(hwnd, NULL, pt.x, pt.y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

typedef struct TASKDIALOGPARAMS
{
    const TASKDIALOGCONFIG *pTaskConfig;
    INT iButton;
    INT iRadioButton;
    BOOL bVerificationChecked;
} TASKDIALOGPARAMS;

#define MAX_BUTTONS 6

static BOOL TaskDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    TASKDIALOGPARAMS *params = (TASKDIALOGPARAMS *)lParam;
    const TASKDIALOGCONFIG *pTaskConfig = params->pTaskConfig;
    HINSTANCE hInstance = pTaskConfig->hInstance;
    PCWSTR pszWindowTitle = pTaskConfig->pszWindowTitle;
    PCWSTR pszMainInstruction = pTaskConfig->pszMainInstruction;
    PCWSTR pszContent = pTaskConfig->pszContent;
    TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons = pTaskConfig->dwCommonButtons;
    PCWSTR pszIcon = pTaskConfig->pszMainIcon;
    LPWSTR psz0, psz1, pszText, pszButton, pch, pszFooter, pszVerification;
    WCHAR szInst[MAX_PATH], szText[MAX_PATH], szButtonText[64], szFooter[MAX_PATH];
    INT i, id, cyRadio, cyCommandLink, cyButtons, cyMinus, cyVerificationText;
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
        GetModuleFileNameW(NULL, szText, ARRAYSIZE(szText));
        psz0 = wcsrchr(szText, L'\\');
        psz1 = wcsrchr(szText, L'/');
        if (!psz0)
            psz0 = psz1;
        if (psz0 < psz1)
            psz0 = psz1;
        pszWindowTitle = psz0 + 1;
    }
    else if (HIWORD(pszWindowTitle) == 0)
    {
        LoadStringW(hInstance, LOWORD(pszWindowTitle), szText, ARRAYSIZE(szText));
        pszWindowTitle = szText;
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
        LoadStringW(hInstance, LOWORD(pszContent), szText, ARRAYSIZE(szText));
        pszContent = szText;
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

    // calculate cyRadio
    GetWindowRect(GetDlgItem(hwnd, rad1), &rc1);
    GetWindowRect(GetDlgItem(hwnd, rad2), &rc2);
    cyRadio = rc2.top - rc1.top;

    // calculate cyCommandLink
    GetWindowRect(GetDlgItem(hwnd, psh1), &rc1);
    GetWindowRect(GetDlgItem(hwnd, psh2), &rc2);
    pt.x = rc1.left;
    pt.y = rc1.top;
    MapWindowPoints(NULL, hwnd, &pt, 1);
    cyCommandLink = rc2.top - rc1.top;

    // calculate cyButtons
    GetWindowRect(GetDlgItem(hwnd, psh1 + MAX_BUTTONS), &rc1);
    cyButtons = rc1.bottom - rc1.top;

    // calculate cyVerificationText
    GetWindowRect(GetDlgItem(hwnd, chx1), &rc1);
    GetWindowRect(GetDlgItem(hwnd, stc2), &rc2);
    cyVerificationText = rc2.top - rc1.top;

    // verification text
    if (pTaskConfig->dwFlags & TDF_VERIFICATION_FLAG_CHECKED)
    {
        CheckDlgButton(hwnd, chx1, BST_CHECKED);
        params->bVerificationChecked = TRUE;
    }
    pszVerification = (LPWSTR)pTaskConfig->pszVerificationText;
    if (pszVerification)
    {
        if (HIWORD(pszVerification) == 0)
        {
            LoadStringW(hInstance, LOWORD(pszVerification), szText, ARRAYSIZE(szText));
            pszVerification = szText;
        }
        SetDlgItemText(hwnd, chx1, pszVerification);

        GetWindowRect(GetDlgItem(hwnd, chx1), &rc1);
        MapWindowRect(NULL, hwnd, &rc1);

        cyMinus = (MAX_BUTTONS - pTaskConfig->cRadioButtons) * cyRadio;

        if (pTaskConfig->dwFlags & TDF_USE_COMMAND_LINKS)
            cyMinus += (MAX_BUTTONS - pTaskConfig->cButtons) * cyCommandLink;
        else
            cyMinus += cyButtons;

        cyMinus += cyButtons;

        MoveWindow(GetDlgItem(hwnd, chx1),
            rc1.left, rc1.top - cyMinus,
            rc1.right - rc1.left, rc1.bottom - rc1.top,
            TRUE);
    }

    // check radio button
    if (!(pTaskConfig->dwFlags & TDF_NO_DEFAULT_RADIO_BUTTON))
    {
        for (i = 0; i < MAX_BUTTONS; ++i)
        {
            if (pTaskConfig->cRadioButtons <= i)
                break;

            if (pTaskConfig->nDefaultRadioButton == pTaskConfig->pRadioButtons[i].nButtonID)
            {
                CheckRadioButton(hwnd, rad1, rad6, rad1 + i);
                break;
            }
        }
        if (i == pTaskConfig->cRadioButtons)
        {
            CheckRadioButton(hwnd, rad1, rad6, rad1);
        }
    }

    // radio
    for (i = 0; i < MAX_BUTTONS; ++i)
    {
        hCtrl = GetDlgItem(hwnd, rad1 + i);

        if (i >= pTaskConfig->cRadioButtons)
        {
            DestroyWindow(hCtrl);
            continue;
        }

        // set text
        pszButton = (LPWSTR)pTaskConfig->pRadioButtons[i].pszButtonText;
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
        id = pTaskConfig->pRadioButtons[i].nButtonID;
        SetWindowLongPtr(hCtrl, GWLP_ID, id);
    }

    // shrink dialog box
    GetWindowRect(hwnd, &rc1);
    if (pTaskConfig->dwFlags & TDF_USE_COMMAND_LINKS)
    {
        rc1.bottom -= (MAX_BUTTONS - pTaskConfig->cRadioButtons) * cyRadio;
        rc1.bottom -= (MAX_BUTTONS - pTaskConfig->cButtons) * cyCommandLink;
        rc1.bottom -= cyButtons;
        if (!pszVerification)
        {
            rc1.bottom -= cyVerificationText;
        }

        for (i = 0; i < MAX_BUTTONS; ++i)
        {
            DestroyWindow(GetDlgItem(hwnd, psh1 + MAX_BUTTONS + i));
        }

        for (i = 0; i < MAX_BUTTONS; ++i)
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

            // move
            GetWindowRect(hCtrl, &rc2);
            MapWindowRect(NULL, hwnd, &rc2);
            MoveWindow(hCtrl,
                rc2.left, rc2.top - (MAX_BUTTONS - pTaskConfig->cRadioButtons) * cyRadio,
                rc2.right - rc2.left, rc2.bottom - rc2.top,
                TRUE);

            // set id
            id = pTaskConfig->pButtons[i].nButtonID;
            SetWindowLongPtr(hCtrl, GWLP_ID, id);
        }
    }
    else
    {
        rc1.bottom -= (MAX_BUTTONS - pTaskConfig->cRadioButtons) * cyRadio;
        rc1.bottom -= MAX_BUTTONS * cyCommandLink;
        if (!pszVerification)
        {
            rc1.bottom -= cyVerificationText;
        }

        for (i = 0; i < MAX_BUTTONS; ++i)
        {
            DestroyWindow(GetDlgItem(hwnd, psh1 + i));
        }

        for (i = 0; i < MAX_BUTTONS; ++i)
        {
            hCtrl = GetDlgItem(hwnd, psh1 + MAX_BUTTONS + i);

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
            pt.y = rc2.top;
            pt.y -= (MAX_BUTTONS - pTaskConfig->cRadioButtons) * cyRadio;
            pt.y -= cyCommandLink * MAX_BUTTONS;
            MoveWindow(hCtrl,
                pt.x, pt.y,
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
    if (pszFooter)
    {
        if (HIWORD(pszFooter) == 0)
        {
            LoadString(hInstance, LOWORD(pszFooter), szFooter, ARRAYSIZE(szFooter));
            pszFooter = szFooter;
        }
        SetWindowText(hStc2, pszFooter);

        if (pTaskConfig->dwFlags & TDF_USE_COMMAND_LINKS)
        {
            cyMinus = cyCommandLink * (MAX_BUTTONS - pTaskConfig->cButtons);
            cyMinus += cyButtons;
            cyMinus += (MAX_BUTTONS - pTaskConfig->cRadioButtons) * cyRadio;
            if (!pszVerification)
            {
                cyMinus += cyVerificationText;
            }
            MoveWindow(hStc2,
                rc2.left, rc2.top - cyMinus,
                rc2.right - rc2.left, rc2.bottom - rc2.top,
                TRUE);
        }
        else
        {
            cyMinus = cyCommandLink * MAX_BUTTONS;
            cyMinus += (MAX_BUTTONS - pTaskConfig->cRadioButtons) * cyRadio;
            if (!pszVerification)
            {
                cyMinus += cyVerificationText;
            }
            MoveWindow(hStc2,
                rc2.left, rc2.top - cyMinus,
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

    DoCenterWindow(hwnd);

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
    TASKDIALOGPARAMS *params = (TASKDIALOGPARAMS *)GetWindowLongPtr(hwnd, DWLP_USER);
    const TASKDIALOGCONFIG *pTaskConfig = params->pTaskConfig;
    INT i, radio_id;

    if (id == chx1)
    {
        params->bVerificationChecked = (IsDlgButtonChecked(hwnd, chx1) == BST_CHECKED);
        return;
    }

    for (i = 0; i < pTaskConfig->cButtons; ++i)
    {
        if (pTaskConfig->pButtons[i].nButtonID == id)
        {
            params->iButton = id;
            EndDialog(hwnd, id);
            break;
        }
    }

    if (id == IDCANCEL)
    {
        if (pTaskConfig->dwFlags & TDF_ALLOW_DIALOG_CANCELLATION)
        {
            params->iButton = id;
            EndDialog(hwnd, id);
        }
    }

    for (i = 0; i < pTaskConfig->cRadioButtons; ++i)
    {
        radio_id = pTaskConfig->pRadioButtons[i].nButtonID;
        if (IsDlgButtonChecked(hwnd, radio_id))
        {
            params->iRadioButton = radio_id;
        }
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
    TASKDIALOGPARAMS params;
    params.pTaskConfig = pTaskConfig;
    params.bVerificationChecked = FALSE;

    if (s_pTaskDialogIndirect && DO_FALLBACK)
    {
        return (*s_pTaskDialogIndirect)(pTaskConfig, pnButton, pnRadioButton,
                                        pfVerificationFlagChecked);
    }

    if (!pTaskConfig || pTaskConfig->cbSize != sizeof(*pTaskConfig) ||
        (pTaskConfig->cButtons && !pTaskConfig->pButtons) ||
        (pTaskConfig->cRadioButtons && !pTaskConfig->pRadioButtons) ||
        (!pTaskConfig->cButtons && pTaskConfig->cRadioButtons))
    {
        return E_INVALIDARG;
    }

    if (!pTaskConfig->cButtons)
    {
        return TaskDialogForXP(pTaskConfig->hwndParent, pTaskConfig->hInstance,
                               pTaskConfig->pszWindowTitle,
                               pTaskConfig->pszMainInstruction,
                               pTaskConfig->pszContent,
                               pTaskConfig->dwCommonButtons,
                               pTaskConfig->pszMainIcon, pnButton);
    }

    DialogBoxParamW(s_hinstDLL, MAKEINTRESOURCEW(IDD_TASKDLG),
                                pTaskConfig->hwndParent, TaskDlgProc, (LPARAM)&params);
    if (pnButton)
    {
        *pnButton = params.iButton;
    }
    if (pnRadioButton)
    {
        *pnRadioButton = params.iRadioButton;
    }
    if (pfVerificationFlagChecked)
    {
        *pfVerificationFlagChecked = params.bVerificationChecked;
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
