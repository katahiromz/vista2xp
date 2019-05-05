/* vista2xp.c
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetver.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <strsafe.h>
#include "resource.h"

HRESULT JustDoIt(HWND hwnd, LPCTSTR pszV2XKRE32, LPCTSTR pszFile);

static HINSTANCE s_hInst;
static TCHAR s_szV2XKER32[MAX_PATH];

typedef BOOL (WINAPI *FN_ChangeWindowMessageFilter)(UINT, DWORD);
static FN_ChangeWindowMessageFilter s_pChangeWindowMessageFilter = NULL;
#ifndef MSGFLT_ADD
    #define MSGFLT_ADD 1
    #define MSGFLT_REMOVE 2
#endif

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    HINSTANCE hKernel32 = GetModuleHandleA("kernel32");

    s_pChangeWindowMessageFilter =
        (FN_ChangeWindowMessageFilter)
            GetProcAddress(hKernel32, "ChangeWindowMessageFilter");

    if (s_pChangeWindowMessageFilter)
        (*s_pChangeWindowMessageFilter)(WM_DROPFILES, MSGFLT_ADD);

    DragAcceptFiles(hwnd, TRUE);
    return TRUE;
}

void AddFolder(HWND hwnd, LPCTSTR pszDir)
{
    TCHAR szCurDir[MAX_PATH], szPath[MAX_PATH];
    HANDLE hFind;
    WIN32_FIND_DATA find;
    DWORD dwType;

    GetCurrentDirectory(ARRAYSIZE(szCurDir), szCurDir);

    if (SetCurrentDirectory(pszDir))
    {
        hFind = FindFirstFile(TEXT("*"), &find);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (lstrcmp(find.cFileName, TEXT(".")) == 0 ||
                    lstrcmp(find.cFileName, TEXT("..")) == 0)
                {
                    continue;
                }

                if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    AddFolder(hwnd, find.cFileName);
                    continue;
                }

                if (!GetBinaryType(find.cFileName, &dwType))
                    continue;

                GetFullPathName(find.cFileName, ARRAYSIZE(szPath), szPath, NULL);

                SendDlgItemMessage(hwnd, lst1, LB_ADDFILE, 0, (LPARAM)szPath);
            } while (FindNextFile(hFind, &find));

            FindClose(hFind);
        }

        SetCurrentDirectory(szCurDir);
    }
}

void AddFile(HWND hwnd, LPCTSTR pszFile)
{
    DWORD dwType;

    if (!PathFileExists(pszFile))
        return;

    if (PathIsDirectory(pszFile))
    {
        AddFolder(hwnd, pszFile);
        return;
    }

    if (!GetBinaryType(pszFile, &dwType))
        return;

    SendDlgItemMessage(hwnd, lst1, LB_ADDFILE, 0, (LPARAM)pszFile);
}

void OnDropFiles(HWND hwnd, HDROP hdrop)
{
    UINT i, nCount;
    TCHAR szPath[MAX_PATH];

    nCount = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);

    for (i = 0; i < nCount; ++i)
    {
        DragQueryFile(hdrop, i, szPath, ARRAYSIZE(szPath));
        AddFile(hwnd, szPath);
    }

    DragFinish(hdrop);
}

void OnPsh1(HWND hwnd)
{
    OPENFILENAME ofn;
    TCHAR szText[128], szPath[MAX_PATH] = TEXT("");

    LoadString(s_hInst, IDS_FILETITLE, szText, ARRAYSIZE(szText));

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = TEXT("Executable Files (*.exe;*.dll)\0*.exe;*.dll\0All Files (*.*)\0*.*\0");
    ofn.lpstrFile = szPath;
    ofn.nMaxFile = ARRAYSIZE(szPath);
    ofn.lpstrTitle = szText;
    ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |
                OFN_HIDEREADONLY;
    ofn.lpstrDefExt = TEXT("exe");

    if (GetOpenFileName(&ofn))
    {
        AddFile(hwnd, szPath);
    }
}

void OnPsh2(HWND hwnd)
{
    TCHAR szText[128], szPath[MAX_PATH];
    BROWSEINFO bi;
    LPITEMIDLIST pidl;

    LoadString(s_hInst, IDS_FOLDERTITLE, szText, ARRAYSIZE(szText));

    ZeroMemory(&bi, sizeof(bi));
    bi.hwndOwner = hwnd;
    bi.lpszTitle = szText;
    bi.ulFlags = BIF_RETURNONLYFSDIRS;
    pidl = SHBrowseForFolder(&bi);
    if (pidl)
    {
        SHGetPathFromIDList(pidl, szPath);
        CoTaskMemFree(pidl);
        AddFolder(hwnd, szPath);
    }
}

void OnPsh3(HWND hwnd)
{
    INT i, nCount;
    INT *pnSel;

    nCount = (INT)SendDlgItemMessage(hwnd, lst1, LB_GETSELCOUNT, 0, 0);
    if (nCount == LB_ERR || nCount == 0)
        return;

    pnSel = (INT *)calloc(nCount, sizeof(INT));
    if (!pnSel)
        return;

    SendDlgItemMessage(hwnd, lst1, LB_GETSELITEMS, nCount, (LPARAM)pnSel);

    for (i = nCount - 1; i >= 0; --i)
    {
        SendDlgItemMessage(hwnd, lst1, LB_DELETESTRING, pnSel[i], 0);
    }
}

void OnOK(HWND hwnd)
{
    TCHAR szText[128], szPath[MAX_PATH];
    INT i, nCount;

    nCount = (INT)SendDlgItemMessage(hwnd, lst1, LB_GETCOUNT, 0, 0);
    if (nCount == LB_ERR || nCount == 0)
    {
        LoadString(s_hInst, IDS_NOITEMS, szText, ARRAYSIZE(szText));
        MessageBox(hwnd, szText, NULL, MB_ICONERROR);
        return;
    }

    HWND hLst1 = GetDlgItem(hwnd, lst1);
    for (i = 0; i < nCount; ++i)
    {
        ListBox_GetText(hLst1, i, szPath);
        if (FAILED(JustDoIt(hwnd, s_szV2XKER32, szPath)))
            return;
    }

    LoadString(s_hInst, IDS_COMPLETE, szText, ARRAYSIZE(szText));
    MessageBox(hwnd, szText, TEXT("vista2xp"), MB_ICONINFORMATION);
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case psh1:
        OnPsh1(hwnd);
        break;
    case psh2:
        OnPsh2(hwnd);
        break;
    case psh3:
        OnPsh3(hwnd);
        break;
    case IDOK:
        OnOK(hwnd);
        break;
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;
    }
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_DROPFILES, OnDropFiles);
    }
    return 0;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    s_hInst = hInstance;
    InitCommonControls();

    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath));
    LPTSTR pch = PathFindFileName(szPath);
    *pch = 0;
    PathAppend(szPath, TEXT("v2xker32.dll"));
    if (!PathFileExists(szPath))
    {
        *pch = 0;
        PathAppend(szPath, TEXT("..\\v2xker32.dll"));
        if (!PathFileExists(szPath))
        {
            *pch = 0;
            PathAppend(szPath, TEXT("..\\..\\v2xker32.dll"));
            if (!PathFileExists(szPath))
            {
                LoadString(NULL, IDS_LOADV2XKER32, szPath, ARRAYSIZE(szPath));
                MessageBox(NULL, szPath, NULL, MB_ICONERROR);
                return -1;
            }
        }
    }
    StringCbCopy(s_szV2XKER32, sizeof(s_szV2XKER32), szPath);

    DialogBox(hInstance, MAKEINTRESOURCE(1), NULL, DialogProc);
    return 0;
}
