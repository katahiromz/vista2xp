/* v2xsh32.c --- v2xsh32.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetver.h"
#include <windows.h>
#include <objbase.h>
#include <cstdio>
#include <shlwapi.h>
#include <shlobj.h>
#include <strsafe.h>
#include "ShellItemArray.hpp"

static HINSTANCE s_hinstDLL;
static HINSTANCE s_hShell32;

typedef HRESULT (STDAPICALLTYPE *FN_SHCreateShellItemArray)(PCIDLIST_ABSOLUTE, IShellFolder *, UINT,
                                                            PCUITEMID_CHILD_ARRAY, IShellItemArray **);
typedef HRESULT (STDAPICALLTYPE *FN_SHCreateShellItemArrayFromDataObject)(IDataObject *, REFIID, void **);
typedef HRESULT (STDAPICALLTYPE *FN_SHCreateShellItemArrayFromIDLists)(UINT, PCIDLIST_ABSOLUTE_ARRAY, IShellItemArray **);
typedef HRESULT (STDAPICALLTYPE *FN_SHCreateShellItemArrayFromShellItem)(IShellItem *, REFIID, void **);
typedef HRESULT (STDAPICALLTYPE *FN_SHCreateItemWithParent)(PCIDLIST_ABSOLUTE, IShellFolder *, PCUITEMID_CHILD, REFIID, void **);
typedef HRESULT (STDAPICALLTYPE *FN_SHCreateItemFromIDList)(PCIDLIST_ABSOLUTE, REFIID, void **);
typedef HRESULT (STDAPICALLTYPE *FN_SHCreateShellItem)(PCIDLIST_ABSOLUTE, IShellFolder *, PCUITEMID_CHILD, IShellItem **);
typedef HRESULT (STDAPICALLTYPE *FN_SHGetNameFromIDList)(PCIDLIST_ABSOLUTE, SIGDN, PWSTR *);

static FN_SHCreateShellItemArray s_pSHCreateShellItemArray;
static FN_SHCreateShellItemArrayFromDataObject s_pSHCreateShellItemArrayFromDataObject;
static FN_SHCreateShellItemArrayFromIDLists s_pSHCreateShellItemArrayFromIDLists;
static FN_SHCreateShellItemArrayFromShellItem s_pSHCreateShellItemArrayFromShellItem;
static FN_SHCreateItemWithParent s_pSHCreateItemWithParent;
static FN_SHCreateItemFromIDList s_pSHCreateItemFromIDList;
static FN_SHCreateShellItem s_pSHCreateShellItem;
static FN_SHGetNameFromIDList s_pSHGetNameFromIDList;

// TODO: SHGetItemFromDataObject, SHGetItemFromObject, SHGetIDListFromObject, SHBindToObject, SHCreateDataObject
// TODO: SHOpenWithDialog

extern "C"
HRESULT STDAPICALLTYPE
SHCreateShellItemForXP(
    PCIDLIST_ABSOLUTE pidlParent,
    IShellFolder      *psfParent,
    PCUITEMID_CHILD   pidl,
    IShellItem        **ppsi)
{
    if (s_pSHCreateShellItem && DO_FALLBACK)
        return (*s_pSHCreateShellItem)(pidlParent, psfParent, pidl, ppsi);

    return SHCreateShellItemForXP0(pidlParent, psfParent, pidl, ppsi);
}

extern "C"
HRESULT STDAPICALLTYPE
SHCreateShellItemArrayForXP(PCIDLIST_ABSOLUTE pidlParent, IShellFolder *psf,
                            UINT cidl, PCUITEMID_CHILD_ARRAY ppidl,
                            IShellItemArray **ppsiItemArray)
{
    if (s_pSHCreateShellItemArray && DO_FALLBACK)
        return (*s_pSHCreateShellItemArray)(pidlParent, psf, cidl, ppidl, ppsiItemArray);

    if (!ppsiItemArray)
        return E_INVALIDARG;

    *ppsiItemArray = NULL;
    MShellItemArray *pArray = MShellItemArray::CreateInstance();
    if (!pArray)
        return E_OUTOFMEMORY;

    for (UINT i = 0; i < cidl; ++i)
    {
        IShellItem *psi = NULL;
        SHCreateShellItemForXP(pidlParent, psf, ppidl[i], &psi);

        if (!psi)
        {
            pArray->Release();
            return E_OUTOFMEMORY;
        }
        pArray->AddItem(psi);
    }

    *ppsiItemArray = pArray;
    return S_OK;
}

extern "C"
HRESULT STDAPICALLTYPE
SHCreateShellItemArrayFromShellItemForXP(IShellItem *psi, REFIID riid, void **ppv)
{
    if (s_pSHCreateShellItemArrayFromShellItem && DO_FALLBACK)
        return (*s_pSHCreateShellItemArrayFromShellItem)(psi, riid, ppv);

    if (!ppv)
        return E_INVALIDARG;

    *ppv = NULL;
    MShellItemArray *pArray = MShellItemArray::CreateInstance();
    if (!pArray)
        return E_OUTOFMEMORY;

    pArray->AddItem(psi);

    *ppv = pArray;
    return S_OK;
}

extern "C"
HRESULT STDAPICALLTYPE
SHCreateShellItemArrayFromDataObjectForXP(IDataObject *pdo, REFIID riid, void **ppv)
{
    if (s_pSHCreateShellItemArrayFromDataObject && DO_FALLBACK)
        return (*s_pSHCreateShellItemArrayFromDataObject)(pdo, riid, ppv);

    if (!ppv)
        return E_INVALIDARG;

    *ppv = NULL;
    MShellItemArray *pArray = MShellItemArray::CreateInstance();
    if (!pArray)
        return E_OUTOFMEMORY;

#ifndef CFSTR_SHELLIDLISTW
    #define CFSTR_SHELLIDLISTW L"Shell IDList Array"
#endif
    STGMEDIUM medium;
    FORMATETC fmt;
    fmt.cfFormat = RegisterClipboardFormatW(CFSTR_SHELLIDLISTW);
    fmt.ptd = NULL;
    fmt.dwAspect = DVASPECT_CONTENT;
    fmt.lindex = -1;
    fmt.tymed = TYMED_HGLOBAL;

    HRESULT hr = pdo->GetData(&fmt, &medium);
    if (SUCCEEDED(hr))
    {
        LPIDA pida = (LPIDA)GlobalLock(medium.hGlobal);
        if (pida)
        {
            LPCITEMIDLIST pidlParent = (LPCITEMIDLIST) ((LPBYTE)pida + pida->aoffset[0]);
            for (UINT i = 0; i < pida->cidl; ++i)
            {
                LPCITEMIDLIST pidlChild = (LPCITEMIDLIST) ((LPBYTE)pida + pida->aoffset[i + 1]);

                IShellItem *psi = NULL;
                SHCreateShellItemForXP(pidlParent, NULL, pidlChild, &psi);
                if (!psi)
                {
                    GlobalUnlock(medium.hGlobal);
                    ReleaseStgMedium(&medium);
                    pArray->Release();
                    return E_OUTOFMEMORY;
                }

                pArray->AddItem(psi);
            }
            GlobalUnlock(medium.hGlobal);
            ReleaseStgMedium(&medium);
        }
    }

    return pArray->QueryInterface(riid, ppv);
}

extern "C"
HRESULT STDAPICALLTYPE
SHCreateShellItemArrayFromIDListsForXP(UINT cidl, PCIDLIST_ABSOLUTE_ARRAY rgpidl,
                                       IShellItemArray **ppsiItemArray)
{
    if (s_pSHCreateShellItemArrayFromIDLists && DO_FALLBACK)
        return (*s_pSHCreateShellItemArrayFromIDLists)(cidl, rgpidl, ppsiItemArray);

    if (!ppsiItemArray)
        return E_INVALIDARG;

    *ppsiItemArray = NULL;
    MShellItemArray *pArray = MShellItemArray::CreateInstance();
    if (!pArray)
        return E_OUTOFMEMORY;

    for (UINT i = 0; i < cidl; ++i)
    {
        IShellItem *psi = NULL;
        SHCreateShellItemForXP(NULL, NULL, rgpidl[i], &psi);

        if (!psi)
        {
            pArray->Release();
            return E_OUTOFMEMORY;
        }
        pArray->AddItem(psi);
    }

    *ppsiItemArray = pArray;
    return S_OK;
}

extern "C"
HRESULT STDAPICALLTYPE
SHCreateItemWithParentForXP(
    PCIDLIST_ABSOLUTE pidlParent,
    IShellFolder      *psfParent,
    PCUITEMID_CHILD   pidl,
    REFIID            riid,
    void              **ppvItem)
{
    if (s_pSHCreateItemWithParent && DO_FALLBACK)
        return (*s_pSHCreateItemWithParent)(pidlParent, psfParent, pidl, riid, ppvItem);

    IShellItem *psi = NULL;
    HRESULT hr = SHCreateShellItemForXP(pidlParent, psfParent, pidl, &psi);
    if (FAILED(hr))
    {
        return hr;
    }

    return psi->QueryInterface(riid, ppvItem);
}

extern "C"
HRESULT STDAPICALLTYPE
SHCreateItemFromIDListForXP(
    PCIDLIST_ABSOLUTE pidl,
    REFIID            riid,
    void              **ppv)
{
    if (s_pSHCreateItemFromIDList && DO_FALLBACK)
        return (*s_pSHCreateItemFromIDList)(pidl, riid, ppv);

    IShellItem *psi = NULL;
    HRESULT hr = SHCreateShellItemForXP(NULL, NULL, pidl, &psi);
    if (FAILED(hr))
    {
        return hr;
    }

    return psi->QueryInterface(riid, ppv);
}

extern "C"
HRESULT STDAPICALLTYPE
SHGetNameFromIDListForXP(PCIDLIST_ABSOLUTE pidl, SIGDN sigdnName, PWSTR *ppszName)
{
    if (s_pSHGetNameFromIDList && DO_FALLBACK)
        return (*s_pSHGetNameFromIDList)(pidl, sigdnName, ppszName);

    return SHGetNameFromIDListForXP0(pidl, sigdnName, ppszName);
}

#define GETPROC(fn) s_p##fn = (FN_##fn)GetProcAddress(s_hShell32, #fn)

extern "C"
BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        s_hinstDLL = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
        s_hShell32 = GetModuleHandleA("shell32");
        GETPROC(SHCreateShellItemArray);
        GETPROC(SHCreateShellItemArrayFromDataObject);
        GETPROC(SHCreateShellItemArrayFromIDLists);
        GETPROC(SHCreateShellItemArrayFromShellItem);
        GETPROC(SHCreateItemWithParent);
        GETPROC(SHCreateItemFromIDList);
        GETPROC(SHCreateShellItem);
        GETPROC(SHGetNameFromIDList);
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
