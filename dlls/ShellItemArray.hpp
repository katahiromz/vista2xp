/* ShellItemArray.hpp --- v2xol.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#ifndef SHELL_ITEM_ARRAY_HPP_
#define SHELL_ITEM_ARRAY_HPP_

#include <windows.h>
#include <objbase.h>
#include <shobjidl.h>
#include <vector>
#include <string>
#include <shlwapi.h>
#include <strsafe.h>
#include <new>

class MEnumShellItems;
class MShellItemArray;

BOOL _ILIsDesktop(LPCITEMIDLIST pidl);

HRESULT STDAPICALLTYPE
SHCreateShellItemForXP0(
    PCIDLIST_ABSOLUTE pidlParent,
    IShellFolder      *psfParent,
    PCUITEMID_CHILD   pidl,
    IShellItem        **ppsi);

HRESULT STDAPICALLTYPE
SHGetNameFromIDListForXP0(PCIDLIST_ABSOLUTE pidl, SIGDN sigdnName, PWSTR *ppszName);

///////////////////////////////////////////////////////////////////////////////

class MShellItem : public IShellItem, public IPersistIDList
{
public:
    static MShellItem *CreateInstance();
    static MShellItem *CreateInstance(LPITEMIDLIST pidl);

    // IUnknown interface
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IShellItem interface
    STDMETHODIMP BindToHandler(IBindCtx *pbc, REFGUID rbhid, REFIID riid, void **ppvOut);
    STDMETHODIMP GetParent(IShellItem **ppsi);
    STDMETHODIMP GetDisplayName(SIGDN sigdnName, LPWSTR *ppszName);
    STDMETHODIMP GetAttributes(SFGAOF sfgaoMask, SFGAOF *psfgaoAttribs);
    STDMETHODIMP Compare(IShellItem *oth, SICHINTF hint, int *piOrder);

    // IPersistIDList interface
    STDMETHODIMP GetClassID(CLSID *pClassID);
    STDMETHODIMP SetIDList(LPCITEMIDLIST pidl);
    STDMETHODIMP GetIDList(LPITEMIDLIST *ppidl);

    HRESULT get_parent_pidl(LPITEMIDLIST *parent_pidl);
    HRESULT get_parent_shellfolder(IShellFolder **ppsf);
    HRESULT get_shellfolder(IBindCtx *pbc, REFIID riid, void **ppvOut);

protected:
    LONG m_nRefCount;
    LPITEMIDLIST m_pidl;

    MShellItem();
    MShellItem(LPITEMIDLIST pidl);
    virtual ~MShellItem();
};

///////////////////////////////////////////////////////////////////////////////

class MEnumShellItems : public IEnumShellItems
{
public:
    static MEnumShellItems *CreateInstance(IShellItemArray *array);

    // IUnknown interface
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IEnumShellItems interface
    STDMETHODIMP Next(
        ULONG celt,
        IShellItem **rgelt,
        ULONG *pceltFetched);
    STDMETHODIMP Skip(ULONG celt);
    STDMETHODIMP Reset();
    STDMETHODIMP Clone(IEnumShellItems **ppenum);

protected:
    LONG m_nRefCount;
    IShellItemArray *m_pArray;
    DWORD m_dwIndex;
    DWORD m_dwCount;

    MEnumShellItems(IShellItemArray *array);
    virtual ~MEnumShellItems();
};

///////////////////////////////////////////////////////////////////////////////

class MShellItemArray : public IShellItemArray
{
public:
    static MShellItemArray *CreateInstance();

    void AddItem(IShellItem *pItem);

    // IUnknown interface
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IShellItemArray interface
    STDMETHODIMP BindToHandler(
        IBindCtx *pbc,
        REFGUID bhid,
        REFIID riid,
        void **ppvOut);
    STDMETHODIMP GetPropertyStore(
        GETPROPERTYSTOREFLAGS flags,
        REFIID riid,
        void **ppv);
    STDMETHODIMP GetPropertyDescriptionList(
        REFPROPERTYKEY keyType,
        REFIID riid,
        void **ppv);
    STDMETHODIMP GetAttributes(
        SIATTRIBFLAGS AttribFlags,
        SFGAOF sfgaoMask,
        SFGAOF *psfgaoAttribs);
    STDMETHODIMP GetCount(DWORD *pdwNumItems);
    STDMETHODIMP GetItemAt(DWORD dwIndex, IShellItem **ppsi);
    STDMETHODIMP EnumItems(IEnumShellItems **ppenumShellItems);

protected:
    LONG m_nRefCount;
    std::vector<IShellItem *> m_items;

    MShellItemArray();
    virtual ~MShellItemArray();
};

///////////////////////////////////////////////////////////////////////////////

#endif  // ndef SHELL_ITEM_ARRAY_HPP_
