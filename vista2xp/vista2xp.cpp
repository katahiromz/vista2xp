/* vista2xp.cpp
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#define _CRT_SECURE_NO_WARNINGS
#include "targetverxp.h"
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <commctrl.h>
#include <commdlg.h>
#include <objbase.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <stdio.h>
#include <assert.h>
#include "resource.h"

#ifndef ARRAYSIZE
    #define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

HRESULT JustDoIt(HWND hwnd, LPCTSTR pszFile);

static HINSTANCE s_hInst;
static TCHAR s_szDLLs[4][MAX_PATH];

static const LPCTSTR s_aapszNames[][3] =
{
    { TEXT("v2xker32.dll"), TEXT("..\\v2xker32.dll"), TEXT("..\\..\\v2xker32.dll") },
    { TEXT("v2xctl32.dll"), TEXT("..\\v2xctl32.dll"), TEXT("..\\..\\v2xctl32.dll") },
    { TEXT("v2xu32.dll"), TEXT("..\\v2xu32.dll"), TEXT("..\\..\\v2xu32.dll") },
    { TEXT("v2xol.dll"), TEXT("..\\v2xol.dll"), TEXT("..\\..\\v2xol.dll") },
    { TEXT("v2xsh32.dll"), TEXT("..\\v2xsh32.dll"), TEXT("..\\..\\v2xsh32.dll") },
    { TEXT("v2xcrt.dll"), TEXT("..\\v2xcrt.dll"), TEXT("..\\..\\v2xcrt.dll") },
};

static const INT s_aids[] =
{
    IDS_LOADV2XKER32,
    IDS_LOADV2XCTL32,
    IDS_LOADV2XU32,
    IDS_LOADV2XOL,
    IDS_LOADV2XSH32,
    IDS_LOADV2XCRT,
};

LPTSTR GetDllSource(INT i)
{
    assert(i < 6);
    return s_szDLLs[i];
}

LPCTSTR GetDllNames(INT i)
{
    assert(i < 6);
    return s_aapszNames[i][0];
}

#ifndef MSGFLT_ADD
    #define MSGFLT_ADD 1
    #define MSGFLT_REMOVE 2
#endif
typedef BOOL (WINAPI *FN_ChangeWindowMessageFilter)(UINT, DWORD);
static FN_ChangeWindowMessageFilter s_pChangeWindowMessageFilter = NULL;

#ifndef MSGFLT_ALLOW
    #define MSGFLT_ALLOW 1
    #define MSGFLT_DISALLOW 2
    #define MSGFLT_RESET 0
    typedef struct tagCHANGEFILTERSTRUCT {
        DWORD cbSize;
        DWORD ExtStatus;
    } CHANGEFILTERSTRUCT, *PCHANGEFILTERSTRUCT;
#endif
#ifndef MSGFLTINFO_NONE
    #define MSGFLTINFO_NONE 0
    #define MSGFLTINFO_ALLOWED_HIGHER 3
    #define MSGFLTINFO_ALREADYALLOWED_FORWND 1
    #define MSGFLTINFO_ALREADYDISALLOWED_FORWND 2
#endif
typedef BOOL (WINAPI *FN_ChangeWindowMessageFilterEx)(HWND, UINT, DWORD, PCHANGEFILTERSTRUCT);
static FN_ChangeWindowMessageFilterEx s_pChangeWindowMessageFilterEx = NULL;

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    HINSTANCE hUser32 = GetModuleHandleA("user32");

    s_pChangeWindowMessageFilterEx =
        (FN_ChangeWindowMessageFilterEx)
            GetProcAddress(hUser32, "ChangeWindowMessageFilterEx");

    s_pChangeWindowMessageFilter =
        (FN_ChangeWindowMessageFilter)
            GetProcAddress(hUser32, "ChangeWindowMessageFilter");

    BOOL ret = FALSE;
    if (s_pChangeWindowMessageFilterEx)
    {
        CHANGEFILTERSTRUCT change;
        change.cbSize = sizeof(change);
        ret = (*s_pChangeWindowMessageFilterEx)(hwnd, WM_DROPFILES, MSGFLT_ALLOW, &change);
        ret = (*s_pChangeWindowMessageFilterEx)(hwnd, 0x0049, MSGFLT_ALLOW, &change);
    }
    else if (s_pChangeWindowMessageFilter)
    {
        ret = (*s_pChangeWindowMessageFilter)(WM_DROPFILES, MSGFLT_ADD);
        ret = (*s_pChangeWindowMessageFilter)(0x0049, MSGFLT_ADD);
    }

    DragAcceptFiles(hwnd, TRUE);
    SendDlgItemMessage(hwnd, lst1, LB_SETHORIZONTALEXTENT, 800, 0);
    return TRUE;
}

BOOL IsBinaryFile(LPCTSTR pszFile)
{
    char buf[3];
    FILE *fp = _tfopen(pszFile, TEXT("rb"));
    if (!fp)
        return FALSE;

    fread(buf, 2, 1, fp);
    fclose(fp);

    return (buf[0] == 'M' && buf[1] == 'Z');
}

void AddFolder(HWND hwnd, LPCTSTR pszDir)
{
    TCHAR szPath[MAX_PATH], szFullPath[MAX_PATH];
    HANDLE hFind;
    WIN32_FIND_DATA find;
    DWORD dwCount;

    dwCount = 0;
    StringCbCopy(szPath, sizeof(szPath), pszDir);
    PathAppend(szPath, TEXT("*"));
    hFind = FindFirstFile(szPath, &find);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (dwCount++ > 64)
                break;

            if (lstrcmp(find.cFileName, TEXT(".")) == 0 ||
                lstrcmp(find.cFileName, TEXT("..")) == 0)
            {
                continue;
            }

            StringCbCopy(szPath, sizeof(szPath), pszDir);
            PathAppend(szPath, find.cFileName);
            GetFullPathName(szPath, ARRAYSIZE(szFullPath), szFullPath, NULL);

            if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                AddFolder(hwnd, szFullPath);
                continue;
            }

            if (!IsBinaryFile(szFullPath))
                continue;

            SendDlgItemMessage(hwnd, lst1, LB_ADDSTRING, 0, (LPARAM)szFullPath);
        } while (FindNextFile(hFind, &find));

        FindClose(hFind);
    }
}

void AddFile(HWND hwnd, LPCTSTR pszFile)
{
    if (PathIsDirectory(pszFile))
    {
        AddFolder(hwnd, pszFile);
        return;
    }

    if (!IsBinaryFile(pszFile))
        return;

    SendDlgItemMessage(hwnd, lst1, LB_ADDSTRING, 0, (LPARAM)pszFile);
}

// get the path of a shortcut file
BOOL GetPathOfShortcutDx(HWND hwnd, LPCWSTR pszLnkFile, LPWSTR pszPath)
{
    BOOL                bRes = FALSE;
    WIN32_FIND_DATAW    find;
    IShellLinkW*        pShellLink;
    IPersistFile*       pPersistFile;
    HRESULT             hRes;

    // NOTE: CoInitialize/CoInitializeEx call required before this
    pszPath[0] = 0;
    hRes = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                            IID_IShellLinkW, (void **)&pShellLink);
    if (SUCCEEDED(hRes))
    {
        hRes = pShellLink->QueryInterface(IID_IPersistFile, (void **)&pPersistFile);
        if (SUCCEEDED(hRes))
        {
            hRes = pPersistFile->Load(pszLnkFile, STGM_READ);
            if (SUCCEEDED(hRes))
            {
                pShellLink->Resolve(hwnd, SLR_NO_UI | SLR_UPDATE);

                hRes = pShellLink->GetPath(pszPath, MAX_PATH, &find, 0);
                if (SUCCEEDED(hRes) && 0 != pszPath[0])
                {
                    bRes = TRUE;
                }
            }
            pPersistFile->Release();
        }
        pShellLink->Release();
    }
    return bRes;
}

void OnDropFiles(HWND hwnd, HDROP hdrop)
{
    UINT i, nCount;
    TCHAR szPath[MAX_PATH], szTarget[MAX_PATH];

    nCount = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);

    for (i = 0; i < nCount; ++i)
    {
        DragQueryFile(hdrop, i, szPath, ARRAYSIZE(szPath));

        if (GetPathOfShortcutDx(hwnd, szPath, szTarget))
        {
            if (PathIsDirectory(szTarget))
                AddFolder(hwnd, szTarget);
            else
                AddFile(hwnd, szTarget);
        }
        else
        {
            if (PathIsDirectory(szTarget))
                AddFolder(hwnd, szPath);
            else
                AddFile(hwnd, szPath);
        }
    }

    DragFinish(hdrop);
}

void OnPsh1(HWND hwnd)
{
    OPENFILENAME ofn;
    TCHAR szText[128], szPath[MAX_PATH] = TEXT("");

    LoadString(NULL, IDS_FILETITLE, szText, ARRAYSIZE(szText));

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

    LoadString(NULL, IDS_FOLDERTITLE, szText, ARRAYSIZE(szText));

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
        LoadString(NULL, IDS_NOITEMS, szText, ARRAYSIZE(szText));
        MessageBox(hwnd, szText, NULL, MB_ICONERROR);
        return;
    }

    HWND hLst1 = GetDlgItem(hwnd, lst1);
    for (i = 0; i < nCount; ++i)
    {
        ListBox_GetText(hLst1, i, szPath);
        if (FAILED(JustDoIt(hwnd, szPath)))
            return;
    }

    LoadString(NULL, IDS_COMPLETE, szText, ARRAYSIZE(szText));
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

BOOL CheckSourceDlls(void)
{
    TCHAR *pch, szPath[MAX_PATH];

    for (size_t i = 0; i < ARRAYSIZE(s_aapszNames); ++i)
    {
        GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath));
        pch = PathFindFileName(szPath);
        *pch = 0;

        PathAppend(szPath, s_aapszNames[i][0]);
        if (!PathFileExists(szPath))
        {
            *pch = 0;
            PathAppend(szPath, s_aapszNames[i][1]);
            if (!PathFileExists(szPath))
            {
                *pch = 0;
                PathAppend(szPath, s_aapszNames[i][2]);
                if (!PathFileExists(szPath))
                {
                    LoadString(NULL, s_aids[i], szPath, ARRAYSIZE(szPath));
                    MessageBox(NULL, szPath, NULL, MB_ICONERROR);
                    return FALSE;
                }
            }
        }
        StringCchCopy(GetDllSource(i), MAX_PATH, szPath);
    }

    return TRUE;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    s_hInst = GetModuleHandleA(NULL);
    InitCommonControls();
    CoInitialize(NULL);

    if (!CheckSourceDlls())
        return -1;

    DialogBox(hInstance, MAKEINTRESOURCE(1), NULL, DialogProc);
    CoUninitialize();
    return 0;
}
