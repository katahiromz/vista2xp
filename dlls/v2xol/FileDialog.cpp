/* FileDialog.cpp --- v2xol.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetver.h"
#include <initguid.h>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <cstdio>
#include <strsafe.h>
#include "FileDialog.hpp"
#include "ShellItemArray.hpp"

#define DOGIF_ONLY_IF_ONE 0x8

DEFINE_GUID(IID_IFileDialog_, 0x42f85136, 0xdb7e, 0x439c, 0x85,0xf1, 0xe4,0x07,0x5d,0x13,0x5f,0xc8);
DEFINE_GUID(IID_IFileOpenDialog_, 0xd57c7288, 0xd4ad, 0x4768, 0xbe,0x02, 0x9d,0x96,0x95,0x32,0xd9,0x60);
DEFINE_GUID(IID_IFileSaveDialog_, 0x84bccd23, 0x5fde, 0x4cdb, 0xae,0xa4, 0xaf,0x64,0xb8,0x3d,0x78,0xab);

static HRESULT SHGetIDListFromObjectForXP(IShellItem *psi, LPITEMIDLIST *ppidl)
{
    if (!ppidl)
        return E_INVALIDARG;

    *ppidl = NULL;

    LPWSTR pszFilePath = NULL;
    HRESULT hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
    if (SUCCEEDED(hr))
    {
        *ppidl = ILCreateFromPathW(pszFilePath);
        if (!*ppidl)
            hr = E_OUTOFMEMORY;
        CoTaskMemFree(pszFilePath);
    }

    return hr;
}

///////////////////////////////////////////////////////////////////////////////

class MFileOpenDialog : public IFileOpenDialog
{
public:
    static MFileOpenDialog *CreateInstance();

    // IUnknown interface
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IModalWindow interface
    STDMETHODIMP Show(HWND hwndOwner);

    // IFileDialog interface
    STDMETHODIMP SetFileTypes(
        UINT cFileTypes,
        const COMDLG_FILTERSPEC *rgFilterSpec);
    STDMETHODIMP SetFileTypeIndex(UINT iFileType);
    STDMETHODIMP GetFileTypeIndex(UINT *piFileType);
    STDMETHODIMP Advise(
        IFileDialogEvents *pfde,
        DWORD *pdwCookie);
    STDMETHODIMP Unadvise(DWORD dwCookie);
    STDMETHODIMP SetOptions(FILEOPENDIALOGOPTIONS fos);
    STDMETHODIMP GetOptions(FILEOPENDIALOGOPTIONS *pfos);
    STDMETHODIMP SetDefaultFolder(IShellItem *psi);
    STDMETHODIMP SetFolder(IShellItem *psi);
    STDMETHODIMP GetFolder(IShellItem **ppsi);
    STDMETHODIMP GetCurrentSelection(IShellItem **ppsi);
    STDMETHODIMP SetFileName(LPCWSTR pszName);
    STDMETHODIMP GetFileName(LPWSTR *pszName);
    STDMETHODIMP SetTitle(LPCWSTR pszTitle);
    STDMETHODIMP SetOkButtonLabel(LPCWSTR pszText);
    STDMETHODIMP SetFileNameLabel(LPCWSTR pszLabel);
    STDMETHODIMP GetResult(IShellItem **ppsi);
    STDMETHODIMP AddPlace(IShellItem *psi, FDAP fdap);
    STDMETHODIMP SetDefaultExtension(LPCWSTR pszDefaultExtension);
    STDMETHODIMP Close(HRESULT hr);
    STDMETHODIMP SetClientGuid(REFGUID guid);
    STDMETHODIMP ClearClientData();
    STDMETHODIMP SetFilter(IShellItemFilter *pFilter);

    // IFileOpenDialog interface
    STDMETHODIMP GetResults(IShellItemArray **ppenum);
    STDMETHODIMP GetSelectedItems(IShellItemArray **ppsai);

protected:
    LONG m_nRefCount;
    HWND m_hwnd;
    LPITEMIDLIST m_pidlSelected;
    LPITEMIDLIST m_pidlDefFolder;
    BOOL m_bDoSave;
    FILEOPENDIALOGOPTIONS m_options;
    LPWSTR m_pszzFiles;
    LPWSTR m_pszTitle;
    LPWSTR m_pszzFilter;
    IFileDialogEvents *m_pEvents;
    DWORD m_dwCookie;
    WCHAR m_szFile[MAX_PATH];
    WCHAR m_szDefExt[32];
    OPENFILENAMEW m_ofn;
    BROWSEINFO m_bi;

    MFileOpenDialog();
    virtual ~MFileOpenDialog();

    IFileDialog *GetFD();
    BOOL UpdateFlags();
    BOOL IsFolderDialog() const;
    LPITEMIDLIST GetFolderIDList();
    BOOL VerifyFileType();

    static UINT_PTR APIENTRY
    OFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

    static int CALLBACK
    BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
};

///////////////////////////////////////////////////////////////////////////////

class MFileSaveDialog : public IFileSaveDialog
{
public:
    static MFileSaveDialog *CreateInstance();

    // IUnknown interface
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IModalWindow interface
    STDMETHODIMP Show(HWND hwndOwner);

    // IFileDialog interface
    STDMETHODIMP SetFileTypes(
        UINT cFileTypes,
        const COMDLG_FILTERSPEC *rgFilterSpec);
    STDMETHODIMP SetFileTypeIndex(UINT iFileType);
    STDMETHODIMP GetFileTypeIndex(UINT *piFileType);
    STDMETHODIMP Advise(
        IFileDialogEvents *pfde,
        DWORD *pdwCookie);
    STDMETHODIMP Unadvise(DWORD dwCookie);
    STDMETHODIMP SetOptions(FILEOPENDIALOGOPTIONS fos);
    STDMETHODIMP GetOptions(FILEOPENDIALOGOPTIONS *pfos);
    STDMETHODIMP SetDefaultFolder(IShellItem *psi);
    STDMETHODIMP SetFolder(IShellItem *psi);
    STDMETHODIMP GetFolder(IShellItem **ppsi);
    STDMETHODIMP GetCurrentSelection(IShellItem **ppsi);
    STDMETHODIMP SetFileName(LPCWSTR pszName);
    STDMETHODIMP GetFileName(LPWSTR *pszName);
    STDMETHODIMP SetTitle(LPCWSTR pszTitle);
    STDMETHODIMP SetOkButtonLabel(LPCWSTR pszText);
    STDMETHODIMP SetFileNameLabel(LPCWSTR pszLabel);
    STDMETHODIMP GetResult(IShellItem **ppsi);
    STDMETHODIMP AddPlace(IShellItem *psi, FDAP fdap);
    STDMETHODIMP SetDefaultExtension(LPCWSTR pszDefaultExtension);
    STDMETHODIMP Close(HRESULT hr);
    STDMETHODIMP SetClientGuid(REFGUID guid);
    STDMETHODIMP ClearClientData();
    STDMETHODIMP SetFilter(IShellItemFilter *pFilter);

    // IFileSaveDialog interface
    STDMETHODIMP SetSaveAsItem(IShellItem *psi);
    STDMETHODIMP SetProperties(IPropertyStore *pStore);
    STDMETHODIMP SetCollectedProperties(
        IPropertyDescriptionList *pList,
        BOOL fAppendDefault);
    STDMETHODIMP GetProperties(IPropertyStore **ppStore);
    STDMETHODIMP ApplyProperties(
        IShellItem *psi,
        IPropertyStore *pStore,
        HWND hwnd,
        IFileOperationProgressSink *pSink);

protected:
    LONG m_nRefCount;
    HWND m_hwnd;
    LPITEMIDLIST m_pidlSelected;
    LPITEMIDLIST m_pidlDefFolder;
    BOOL m_bDoSave;
    FILEOPENDIALOGOPTIONS m_options;
    LPWSTR m_pszzFiles;
    LPWSTR m_pszTitle;
    LPWSTR m_pszzFilter;
    IFileDialogEvents *m_pEvents;
    DWORD m_dwCookie;
    WCHAR m_szFile[MAX_PATH];
    WCHAR m_szDefExt[32];
    OPENFILENAMEW m_ofn;
    BROWSEINFO m_bi;

    MFileSaveDialog();
    virtual ~MFileSaveDialog();

    BOOL UpdateFlags();
    BOOL IsFolderDialog() const;
    LPITEMIDLIST GetFolderIDList();
    BOOL VerifyFileType();

    static UINT_PTR APIENTRY
    OFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

    static int CALLBACK
    BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
};

///////////////////////////////////////////////////////////////////////////////

IFileOpenDialog *createFileOpenDialog(void)
{
    return MFileOpenDialog::CreateInstance();
}

IFileSaveDialog *createFileSaveDialog(void)
{
    return MFileSaveDialog::CreateInstance();
}

///////////////////////////////////////////////////////////////////////////////
// MFileOpenDialog implementation

#undef THIS_CLASS
#define THIS_CLASS MFileOpenDialog
#include "FileDialog_common.hpp"

THIS_CLASS::THIS_CLASS() :
    m_nRefCount(1),
    m_hwnd(NULL),
    m_pidlSelected(NULL),
    m_pidlDefFolder(NULL),
    m_bDoSave(FALSE),
    m_options(0),
    m_pszzFiles(NULL),
    m_pszTitle(NULL),
    m_pszzFilter(NULL),
    m_pEvents(NULL),
    m_dwCookie(0)
{
    ZeroMemory(&m_ofn, sizeof(m_ofn));
    m_ofn.lStructSize = sizeof(m_ofn);
    m_ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_ENABLEHOOK;
    m_ofn.lpfnHook = OFNHookProc;
    m_ofn.lCustData = (LPARAM)this;

    ZeroMemory(&m_bi, sizeof(m_bi));
    m_bi.ulFlags |= BIF_EDITBOX | BIF_USENEWUI | BIF_NEWDIALOGSTYLE | BIF_STATUSTEXT;
    m_bi.lpfn = BrowseCallbackProc;
    m_bi.lParam = (LPARAM)this;

    m_szFile[0] = 0;
    m_szDefExt[0] = 0;

    m_bDoSave = FALSE;
    m_options = FOS_NOCHANGEDIR | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST;
}

STDMETHODIMP THIS_CLASS::QueryInterface(REFIID riid, void **ppvObj)
{
    if (riid == IID_IUnknown || riid == IID_IFileDialog_ ||
        riid == IID_IModalWindow || riid == IID_IFileOpenDialog_)
    {
        *ppvObj = static_cast<IFileOpenDialog *>(this);
    }
    else
    {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

// IFileOpenDialog interface

STDMETHODIMP THIS_CLASS::GetResults(IShellItemArray **ppenum)
{
    if (!ppenum || !(m_options & FOS_ALLOWMULTISELECT))
        return E_INVALIDARG;

    if (!m_pszzFiles || !*m_pszzFiles)
        return E_FAIL;

    MShellItemArray *pArray = MShellItemArray::CreateInstance();
    if (!pArray)
    {
        return E_OUTOFMEMORY;
    }

    WCHAR szPath[MAX_PATH];
    LPWSTR pchTitle = NULL;
    for (LPWSTR pch = m_pszzFiles; pch && *pch; pch += lstrlenW(pch) + 1)
    {
        if (pch == m_pszzFiles)
        {
            StringCbCopyW(szPath, sizeof(szPath), pch);
            pchTitle = szPath + lstrlenW(szPath);
            continue;
        }
        else
        {
            *pchTitle = 0;
            PathAppendW(szPath, pch);
        }

        LPITEMIDLIST pidl = ILCreateFromPathW(szPath);
        IShellItem *psi = NULL;
        ::SHCreateShellItem(NULL, NULL, pidl, &psi);
        CoTaskMemFree(pidl);

        if (!psi)
        {
            pArray->Release();
            return E_OUTOFMEMORY;
        }

        pArray->AddItem(psi);
        psi->Release();
    }

    *ppenum = pArray;

    return S_OK;
}

STDMETHODIMP THIS_CLASS::GetSelectedItems(IShellItemArray **ppsai)
{
    return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////////
// MFileSaveDialog implementation

#undef THIS_CLASS
#define THIS_CLASS MFileSaveDialog
#include "FileDialog_common.hpp"

THIS_CLASS::THIS_CLASS() :
    m_nRefCount(1),
    m_hwnd(NULL),
    m_pidlSelected(NULL),
    m_pidlDefFolder(NULL),
    m_bDoSave(FALSE),
    m_options(0),
    m_pszzFiles(NULL),
    m_pszTitle(NULL),
    m_pszzFilter(NULL),
    m_pEvents(NULL),
    m_dwCookie(0)
{
    ZeroMemory(&m_ofn, sizeof(m_ofn));
    m_ofn.lStructSize = sizeof(m_ofn);
    m_ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_ENABLEHOOK;
    m_ofn.lpfnHook = OFNHookProc;
    m_ofn.lCustData = (LPARAM)this;

    ZeroMemory(&m_bi, sizeof(m_bi));
    m_bi.ulFlags |= BIF_EDITBOX | BIF_USENEWUI | BIF_NEWDIALOGSTYLE | BIF_STATUSTEXT;
    m_bi.lpfn = BrowseCallbackProc;
    m_bi.lParam = (LPARAM)this;

    m_szFile[0] = 0;
    m_szDefExt[0] = 0;

    m_bDoSave = TRUE;
    m_options = FOS_OVERWRITEPROMPT | FOS_NOCHANGEDIR |
                FOS_PATHMUSTEXIST | FOS_NOREADONLYRETURN;
}

// IUnknown interface

STDMETHODIMP THIS_CLASS::QueryInterface(REFIID riid, void **ppvObj)
{
    if (riid == IID_IUnknown || riid == IID_IFileDialog_ ||
        riid == IID_IModalWindow || riid == IID_IFileSaveDialog_)
    {
        *ppvObj = static_cast<IFileSaveDialog *>(this);
    }
    else
    {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

// IFileSaveDialog interface

STDMETHODIMP THIS_CLASS::SetSaveAsItem(IShellItem *psi)
{
    if (!psi)
        return E_INVALIDARG;

    LPITEMIDLIST pidl = NULL;
    SHGetIDListFromObjectForXP(psi, &pidl);

    WCHAR szPath[MAX_PATH];
    SHGetPathFromIDListW(pidl, szPath);
    SetFileName(szPath);

    return S_OK;
}

STDMETHODIMP THIS_CLASS::SetProperties(IPropertyStore *pStore)
{
    return E_NOTIMPL;
}

STDMETHODIMP THIS_CLASS::SetCollectedProperties(
    IPropertyDescriptionList *pList,
    BOOL fAppendDefault)
{
    return E_NOTIMPL;
}

STDMETHODIMP THIS_CLASS::GetProperties(IPropertyStore **ppStore)
{
    return E_NOTIMPL;
}

STDMETHODIMP THIS_CLASS::ApplyProperties(
    IShellItem *psi,
    IPropertyStore *pStore,
    HWND hwnd,
    IFileOperationProgressSink *pSink)
{
    return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////////

#undef THIS_CLASS
