/* ShellItemArray.cpp --- v2xol.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */

#include "targetverxp.h"
#include <initguid.h>
#include "ShellItemArray.hpp"
#include <shlobj.h>
#include <new>
#include <shlwapi.h>
#include <strsafe.h>

DEFINE_GUID(IID_IUnknown_, 0x00000000, 0x0000, 0x0000, 0xc0,0x00, 0x00,0x00,0x00,0x00,0x00,0x46);
DEFINE_GUID(IID_IShellItem_, 0x43826d1e, 0xe718, 0x42ee, 0xbc,0x55, 0xa1,0xe2,0x61,0xc3,0x7b,0xfe);
DEFINE_GUID(IID_IShellItemArray_, 0xb63ea76d, 0x1f85, 0x456f, 0xa1,0x9c, 0x48,0x15,0x9e,0xfa,0x85,0x8b);
DEFINE_GUID(IID_IPersistIDList_, 0x1079acfc, 0x29bd, 0x11d3, 0x8e,0x0d, 0x00,0xc0,0x4f,0x68,0x37,0xd5);
DEFINE_GUID(IID_IEnumShellItems_, 0x70629033, 0xe363, 0x4a28, 0xa5,0x67, 0x0d,0xb7,0x80,0x06,0xe6,0xd7);
DEFINE_GUID(BHID_SFObject_, 0x3981e224, 0xf559, 0x11d3, 0x8e, 0x3a, 0x00, 0xc0, 0x4f, 0x68, 0x37, 0xd5);
DEFINE_GUID(BHID_SFUIObject_, 0x3981e225, 0xf559, 0x11d3, 0x8e, 0x3a, 0x00, 0xc0, 0x4f, 0x68, 0x37, 0xd5);
DEFINE_GUID(BHID_SFViewObject_, 0x3981e226, 0xf559, 0x11d3, 0x8e, 0x3a, 0x00, 0xc0, 0x4f, 0x68, 0x37, 0xd5);
DEFINE_GUID(BHID_DataObject_, 0xb8c0bd9f, 0xed24, 0x455c, 0x83, 0xe6, 0xd5, 0x39, 0xc, 0x4f, 0xe8, 0xc4);
DEFINE_GUID(CLSID_ShellItem_, 0x9ac9fbe1, 0xe0a2, 0x4ad6, 0xb4,0xee, 0xe2,0x12,0x01,0x3e,0xa9,0x17);

///////////////////////////////////////////////////////////////////////////////

BOOL _ILIsDesktop(LPCITEMIDLIST pidl)
{
    return !pidl || !pidl->mkid.cb;
}

HRESULT STDAPICALLTYPE
SHCreateShellItemForXP0(
    PCIDLIST_ABSOLUTE pidlParent,
    IShellFolder      *psfParent,
    PCUITEMID_CHILD   pidl,
    IShellItem        **ppsi)
{
    if (!ppsi || !pidl)
        return E_INVALIDARG;

    *ppsi = NULL;
    LPITEMIDLIST new_pidl = NULL;
    HRESULT hr;
    if (pidlParent || psfParent)
    {
        LPITEMIDLIST temp_parent = NULL;
        if (!pidlParent)
        {
            IPersistFolder2 *ppf2Parent = NULL;
            hr = psfParent->QueryInterface(IID_IPersistFolder2, (void **)&ppf2Parent);
            if (FAILED(hr))
            {
                return E_NOINTERFACE;
            }

            hr = ppf2Parent->GetCurFolder(&temp_parent);
            if (FAILED(hr))
            {
                ppf2Parent->Release();
                return E_NOINTERFACE;
            }

            pidlParent = temp_parent;
            ppf2Parent->Release();
        }

        new_pidl = ILCombine(pidlParent, pidl);
        ILFree(temp_parent);

        if (!new_pidl)
            return E_OUTOFMEMORY;
    }
    else
    {
        new_pidl = ILClone(pidl);
        if (!new_pidl)
            return E_OUTOFMEMORY;
    }

    IShellItem *pItem = MShellItem::CreateInstance();
    if (pItem)
    {
        IPersistIDList *pIDList = NULL;
        hr = pItem->QueryInterface(IID_IPersistIDList_, (void **)&pIDList);
        if (SUCCEEDED(hr))
        {
            hr = pIDList->SetIDList(new_pidl);
            pIDList->Release();
        }
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }
    ILFree(new_pidl);

    if (SUCCEEDED(hr))
    {
        *ppsi = pItem;
    }
    else
    {
        pItem->Release();
    }

    return hr;
}

HRESULT STDAPICALLTYPE
SHGetNameFromIDListForXP0(PCIDLIST_ABSOLUTE pidl, SIGDN sigdnName, PWSTR *ppszName)
{
    IShellFolder *psfparent;
    LPCITEMIDLIST child_pidl;
    STRRET disp_name;
    HRESULT ret;

    *ppszName = NULL;
    ret = SHBindToParent(pidl, IID_IShellFolder, (void**)&psfparent, &child_pidl);
    if (SUCCEEDED(ret))
    {
        switch (sigdnName)
        {
        case SIGDN_NORMALDISPLAY:               /* SHGDN_NORMAL */
        case SIGDN_PARENTRELATIVEPARSING:       /* SHGDN_INFOLDER | SHGDN_FORPARSING */
        case SIGDN_PARENTRELATIVEEDITING:       /* SHGDN_INFOLDER | SHGDN_FOREDITING */
        case SIGDN_DESKTOPABSOLUTEPARSING:      /* SHGDN_FORPARSING */
        case SIGDN_DESKTOPABSOLUTEEDITING:      /* SHGDN_FOREDITING | SHGDN_FORADDRESSBAR*/
        case SIGDN_PARENTRELATIVEFORADDRESSBAR: /* SIGDN_INFOLDER | SHGDN_FORADDRESSBAR */
        case SIGDN_PARENTRELATIVE:              /* SIGDN_INFOLDER */
            disp_name.uType = STRRET_WSTR;
            ret = psfparent->GetDisplayNameOf(child_pidl, (sigdnName & 0xffff), &disp_name);
            if (SUCCEEDED(ret))
                ret = StrRetToStrW(&disp_name, pidl, ppszName);
            break;

        case SIGDN_FILESYSPATH:
            *ppszName = (LPWSTR)CoTaskMemAlloc(sizeof(WCHAR) * MAX_PATH);
            if (SHGetPathFromIDListW(pidl, *ppszName))
            {
                ret = S_OK;
            }
            else
            {
                CoTaskMemFree(*ppszName);
                ret = E_INVALIDARG;
            }
            break;

        case SIGDN_URL:
        default:
            ret = E_FAIL;
            break;
        }

        psfparent->Release();
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////////////

/*static*/ MShellItem *MShellItem::CreateInstance()
{
    return new(std::nothrow) MShellItem();
}

/*static*/ MShellItem *MShellItem::CreateInstance(LPITEMIDLIST pidl)
{
    return new(std::nothrow) MShellItem(pidl);
}

MShellItem::MShellItem() : m_nRefCount(1), m_pidl(NULL)
{
}

MShellItem::MShellItem(LPITEMIDLIST pidl) : m_nRefCount(1), m_pidl(NULL)
{
    SetIDList(pidl);
}

MShellItem::~MShellItem()
{
    CoTaskMemFree(m_pidl);
}

HRESULT MShellItem::get_parent_pidl(LPITEMIDLIST *parent_pidl)
{
    *parent_pidl = ILClone(m_pidl);
    if (*parent_pidl)
    {
        if (ILRemoveLastID(*parent_pidl))
        {
            return S_OK;
        }
        else
        {
            ILFree(*parent_pidl);
            *parent_pidl = NULL;
            return E_INVALIDARG;
        }
    }
    else
    {
        *parent_pidl = NULL;
        return E_OUTOFMEMORY;
    }
}

HRESULT MShellItem::get_parent_shellfolder(IShellFolder **ppsf)
{
    HRESULT hr;
    LPITEMIDLIST parent_pidl;
    IShellFolder *desktop = NULL;

    hr = get_parent_pidl(&parent_pidl);
    if (SUCCEEDED(hr))
    {
        hr = SHGetDesktopFolder(&desktop);
        if (SUCCEEDED(hr))
            hr = desktop->BindToObject(parent_pidl, NULL, IID_IShellFolder, (void **)ppsf);

        ILFree(parent_pidl);
        desktop->Release();
    }

    return hr;
}

HRESULT MShellItem::get_shellfolder(IBindCtx *pbc, REFIID riid, void **ppvOut)
{
    IShellFolder *psf = NULL;
    IShellFolder *psfDesktop = NULL;
    HRESULT hr;

    hr = SHGetDesktopFolder(&psfDesktop);
    if (FAILED(hr))
        return hr;

    if (_ILIsDesktop(m_pidl))
    {
        psf = psfDesktop;
        psf->AddRef();
    }
    else
    {
        hr = psfDesktop->BindToObject(m_pidl, pbc, IID_IShellFolder, (void **)&psf);
        if (FAILED(hr))
        {
            psfDesktop->Release();
            return hr;
        }
    }

    hr = psf->QueryInterface(riid, ppvOut);

    psf->Release();
    psfDesktop->Release();
    return hr;
}

// IUnknown interface

STDMETHODIMP MShellItem::QueryInterface(REFIID riid, void **ppvObject)
{
    if (IsEqualIID(riid, IID_IUnknown_) || IsEqualIID(riid, IID_IShellItem_))
    {
        *ppvObject = static_cast<IShellItem *>(this);
    }
    else if (IsEqualIID(riid, IID_IPersistIDList_))
    {
        *ppvObject = static_cast<IPersistIDList *>(this);
    }
    else
    {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) MShellItem::AddRef()
{
    m_nRefCount++;
    return m_nRefCount;
}

STDMETHODIMP_(ULONG) MShellItem::Release()
{
    --m_nRefCount;
    if (m_nRefCount != 0)
        return m_nRefCount;

    delete this;
    return 0;
}

// IShellItem interface

STDMETHODIMP
MShellItem::BindToHandler(IBindCtx *pbc, REFGUID rbhid, REFIID riid, void **ppvOut)
{
    HRESULT hr;
    if (IsEqualIID(rbhid, BHID_SFObject_))
    {
        return get_shellfolder(pbc, riid, ppvOut);
    }
    else if (IsEqualIID(rbhid, BHID_SFUIObject_))
    {
        IShellFolder *pShellFolder = NULL;
        if (_ILIsDesktop(m_pidl))
            hr = SHGetDesktopFolder(&pShellFolder);
        else
            hr = get_parent_shellfolder(&pShellFolder);

        if (FAILED(hr))
            return hr;

        LPCITEMIDLIST pidl = ILFindLastID(m_pidl);
        hr = pShellFolder->GetUIObjectOf(NULL, 1, &pidl, riid, NULL, ppvOut);
        pShellFolder->Release();
        return hr;
    }
    else if (IsEqualIID(rbhid, BHID_DataObject_))
    {
        return BindToHandler(pbc, BHID_SFUIObject_, IID_IDataObject, ppvOut);
    }
    else if (IsEqualIID(rbhid, BHID_SFViewObject_))
    {
        IShellFolder *psf = NULL;
        hr = get_shellfolder(NULL, IID_IShellFolder, (void **)&psf);
        if (FAILED(hr))
            return hr;

        hr = psf->CreateViewObject(NULL, riid, ppvOut);
        psf->Release();
        return hr;
    }

    return MK_E_NOOBJECT;
}

STDMETHODIMP MShellItem::GetParent(IShellItem **ppsi)
{
    HRESULT hr;
    LPITEMIDLIST parent_pidl;

    if (!ppsi)
        return E_INVALIDARG;

    *ppsi = NULL;

    hr = get_parent_pidl(&parent_pidl);
    if (SUCCEEDED(hr))
    {
        *ppsi = MShellItem::CreateInstance(parent_pidl);
        ILFree(parent_pidl);
    }

    if (!ppsi)
        return E_OUTOFMEMORY;

    return S_OK;
}

STDMETHODIMP MShellItem::GetDisplayName(SIGDN sigdnName, LPWSTR *ppszName)
{
    return SHGetNameFromIDListForXP0(m_pidl, sigdnName, ppszName);
}

STDMETHODIMP MShellItem::GetAttributes(SFGAOF sfgaoMask, SFGAOF *psfgaoAttribs)
{
    IShellFolder *parent_folder = NULL;
    LPCITEMIDLIST child_pidl;
    HRESULT hr;

    if (_ILIsDesktop(m_pidl))
        hr = SHGetDesktopFolder(&parent_folder);
    else
        hr = get_parent_shellfolder(&parent_folder);
    if (FAILED(hr))
        return hr;

    child_pidl = ILFindLastID(m_pidl);
    *psfgaoAttribs = sfgaoMask;
    hr = parent_folder->GetAttributesOf(1, &child_pidl, psfgaoAttribs);
    *psfgaoAttribs &= sfgaoMask;

    parent_folder->Release();

    if (FAILED(hr))
        return hr;

    return (sfgaoMask == *psfgaoAttribs) ? S_OK : S_FALSE;
}

STDMETHODIMP MShellItem::Compare(IShellItem *oth, SICHINTF hint, int *piOrder)
{
    HRESULT hr;
    IPersistIDList *pIDList = NULL;
    IShellFolder *psfDesktop = NULL;
    LPITEMIDLIST pidl;

    if (piOrder == NULL || oth == NULL)
        return E_POINTER;

    hr = oth->QueryInterface(IID_IPersistIDList_, (void **)&pIDList);
    if (SUCCEEDED(hr))
    {
        hr = pIDList->GetIDList(&pidl);
        if (SUCCEEDED(hr))
        {
            hr = SHGetDesktopFolder(&psfDesktop);
            if (SUCCEEDED(hr))
            {
                hr = psfDesktop->CompareIDs(hint, m_pidl, pidl);
                *piOrder = (int)(short)SCODE_CODE(hr);
            }
            ILFree(pidl);
        }
    }

    if (SUCCEEDED(hr))
    {
        if (*piOrder)
            hr = S_FALSE;
        else
            hr = S_OK;

    }

    if (pIDList)
        pIDList->Release();
    if (psfDesktop)
        psfDesktop->Release();

    return hr;
}

// IPersistIDList interface

STDMETHODIMP MShellItem::GetClassID(CLSID *pClassID)
{
    *pClassID = CLSID_ShellItem_;
    return S_OK;
}

STDMETHODIMP MShellItem::SetIDList(LPCITEMIDLIST pidl)
{
    LPITEMIDLIST new_pidl;

    new_pidl = ILClone(pidl);
    if (new_pidl)
    {
        ILFree(m_pidl);
        m_pidl = new_pidl;
        return S_OK;
    }
    return E_OUTOFMEMORY;
}

STDMETHODIMP MShellItem::GetIDList(LPITEMIDLIST *ppidl)
{
    if (!ppidl)
        return E_INVALIDARG;

    *ppidl = ILClone(m_pidl);
    if (*ppidl)
        return S_OK;
    else
        return E_OUTOFMEMORY;
}

///////////////////////////////////////////////////////////////////////////////

MEnumShellItems::MEnumShellItems(IShellItemArray *array) :
    m_nRefCount(1),
    m_pArray(array),
    m_dwIndex(0)
{
    if (m_pArray)
    {
        m_pArray->AddRef();
        m_pArray->GetCount(&m_dwCount);
    }
}

MEnumShellItems::~MEnumShellItems()
{
    if (m_pArray)
    {
        m_pArray->Release();
        m_pArray = NULL;
    }
}

/*static*/ MEnumShellItems *
MEnumShellItems::CreateInstance(IShellItemArray *array)
{
    return new(std::nothrow) MEnumShellItems(array);
}

// IUnknown interface

STDMETHODIMP MEnumShellItems::QueryInterface(REFIID riid, void **ppvObject)
{
    if (IsEqualIID(riid, IID_IUnknown_) || IsEqualIID(riid, IID_IEnumShellItems_))
    {
        *ppvObject = static_cast<IEnumShellItems *>(this);
    }
    else
    {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) MEnumShellItems::AddRef()
{
    m_nRefCount++;
    return m_nRefCount;
}

STDMETHODIMP_(ULONG) MEnumShellItems::Release()
{
    --m_nRefCount;
    if (m_nRefCount != 0)
        return m_nRefCount;

    delete this;
    return 0;
}

// IEnumShellItems interface

STDMETHODIMP MEnumShellItems::Next(
    ULONG celt,
    IShellItem **rgelt,
    ULONG *pceltFetched)
{
    HRESULT hr;
    DWORD i;
    ULONG fetched = 0;

    if (!m_pArray)
        return E_FAIL;

    if (pceltFetched == NULL && celt != 1)
        return E_INVALIDARG;

    for (i = m_dwIndex; fetched < celt && i < m_dwCount; i++)
    {
        hr = m_pArray->GetItemAt(i, &rgelt[fetched]);
        if (FAILED(hr))
            break;
        fetched++;
        ++m_dwIndex;
    }

    if (SUCCEEDED(hr))
    {
        if (pceltFetched != NULL)
            *pceltFetched = fetched;

        if (fetched > 0)
            return S_OK;

        return S_FALSE;
    }

    return hr;
}

STDMETHODIMP MEnumShellItems::Skip(ULONG celt)
{
    if (m_dwIndex + celt < m_dwCount - 1)
        m_dwIndex = m_dwIndex + celt;
    else
        m_dwIndex = m_dwCount - 1;
    return S_OK;
}

STDMETHODIMP MEnumShellItems::Reset()
{
    m_dwIndex = 0;
    return S_OK;
}

STDMETHODIMP MEnumShellItems::Clone(IEnumShellItems **ppenum)
{
    if (!ppenum)
        return E_INVALIDARG;

    if (!m_pArray)
        return E_FAIL;

    MEnumShellItems *pEnum = MEnumShellItems::CreateInstance(m_pArray);
    if (!pEnum)
        return E_OUTOFMEMORY;

    pEnum->m_dwIndex = m_dwIndex;
    *ppenum = pEnum;
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////

MShellItemArray::MShellItemArray() :
    m_nRefCount(1),
    m_nItemCount(0),
    m_pItems(NULL)
{
}

MShellItemArray::~MShellItemArray()
{
    for (size_t i = 0; i < m_nItemCount; ++i)
    {
        m_pItems[i]->Release();
    }
    CoTaskMemFree(m_pItems);
}

/*static*/ MShellItemArray *
MShellItemArray::CreateInstance()
{
    return new(std::nothrow) MShellItemArray();
}

HRESULT MShellItemArray::AddItem(IShellItem *pItem)
{
    if (!pItem)
        return E_INVALIDARG;

    ULONG nNewCount = m_nItemCount + 1;
    IShellItem **pNewItems;
    pNewItems = (IShellItem **)CoTaskMemAlloc(nNewCount * sizeof(IShellItem *));
    if (!pNewItems)
        return E_OUTOFMEMORY;

    CopyMemory(pNewItems, m_pItems, m_nItemCount * sizeof(IShellItem *));
    CoTaskMemFree(m_pItems);
    m_pItems = pNewItems;

    m_pItems[m_nItemCount] = pItem;
    pItem->AddRef();
    return S_OK;
}

STDMETHODIMP MShellItemArray::QueryInterface(REFIID riid, void **ppvObject)
{
    if (IsEqualIID(riid, IID_IUnknown_) || IsEqualIID(riid, IID_IShellItemArray_))
    {
        *ppvObject = static_cast<IShellItemArray *>(this);
    }
    else
    {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) MShellItemArray::AddRef()
{
    m_nRefCount++;
    return m_nRefCount;
}

STDMETHODIMP_(ULONG) MShellItemArray::Release()
{
    --m_nRefCount;
    if (m_nRefCount != 0)
        return m_nRefCount;

    delete this;
    return 0;
}

STDMETHODIMP MShellItemArray::BindToHandler(
    IBindCtx *pbc,
    REFGUID bhid,
    REFIID riid,
    void **ppvOut)
{
    return E_NOTIMPL;
}

STDMETHODIMP MShellItemArray::GetPropertyStore(
    GETPROPERTYSTOREFLAGS flags,
    REFIID riid,
    void **ppv)
{
    return E_NOTIMPL;
}

STDMETHODIMP MShellItemArray::GetPropertyDescriptionList(
    REFPROPERTYKEY keyType,
    REFIID riid,
    void **ppv)
{
    return E_NOTIMPL;
}

STDMETHODIMP MShellItemArray::GetAttributes(
    SIATTRIBFLAGS AttribFlags,
    SFGAOF sfgaoMask,
    SFGAOF *psfgaoAttribs)
{
    HRESULT hr;
    SFGAOF attr;

    for (ULONG i = 0; i < m_nItemCount; ++i)
    {
        hr = m_pItems[i]->GetAttributes(sfgaoMask, &attr);
        if (FAILED(hr))
            break;

        if (i == 0)
        {
            *psfgaoAttribs = attr;
            continue;
        }

        switch (AttribFlags & SIATTRIBFLAGS_MASK)
        {
        case SIATTRIBFLAGS_AND:
            *psfgaoAttribs &= attr;
            break;
        case SIATTRIBFLAGS_OR:
            *psfgaoAttribs |= attr;
            break;
        }
    }

    if (SUCCEEDED(hr))
    {
        if (*psfgaoAttribs == sfgaoMask)
            return S_OK;

        return S_FALSE;
    }

    return hr;
}

STDMETHODIMP MShellItemArray::GetCount(DWORD *pdwNumItems)
{
    if (!pdwNumItems)
        return E_INVALIDARG;

    *pdwNumItems = m_nItemCount;
    return S_OK;
}

STDMETHODIMP MShellItemArray::GetItemAt(DWORD dwIndex, IShellItem **ppsi)
{
    if (!ppsi)
        return E_INVALIDARG;

    if (dwIndex < m_nItemCount)
    {
        *ppsi = m_pItems[dwIndex];
        m_pItems[dwIndex]->AddRef();
        return S_OK;
    }

    return E_FAIL;
}

STDMETHODIMP MShellItemArray::EnumItems(IEnumShellItems **ppenumShellItems)
{
    if (!ppenumShellItems)
        return E_INVALIDARG;

    *ppenumShellItems = MEnumShellItems::CreateInstance(this);
    if (!*ppenumShellItems)
        return E_OUTOFMEMORY;

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
