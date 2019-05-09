/* ShellItemArray.cpp --- v2xol.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */

#include "ShellItemArray.hpp"
#include <shlobj.h>

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
        hr = pItem->QueryInterface(IID_IPersistIDList, (void **)&pIDList);
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
    if(SUCCEEDED(ret))
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
            ret = psfparent->GetDisplayNameOf(child_pidl,
                                              sigdnName & 0xffff,
                                              &disp_name);
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
    if (riid == IID_IUnknown || riid == IID_IShellItem)
    {
        *ppvObject = static_cast<IShellItem *>(this);
    }
    else if (riid == IID_IPersistIDList)
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
    if (rbhid == BHID_SFObject)
    {
        return get_shellfolder(pbc, riid, ppvOut);
    }
    else if (rbhid == BHID_SFUIObject)
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
    else if (rbhid == BHID_DataObject)
    {
        return BindToHandler(pbc, BHID_SFUIObject, IID_IDataObject, ppvOut);
    }
    else if (rbhid == BHID_SFViewObject)
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

    hr = oth->QueryInterface(IID_IPersistIDList, (void **)&pIDList);
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
    *pClassID = CLSID_ShellItem;
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
    if (riid == IID_IUnknown || riid == IID_IEnumShellItems)
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
}

STDMETHODIMP MEnumShellItems::Clone(IEnumShellItems **ppenum)
{
    if (!ppenum)
        return E_INVALIDARG;

    if (!m_pArray)
        return E_FAIL;

    MEnumShellItems *pEnum = CreateInstance(m_pArray);
    if (!pEnum)
        return E_OUTOFMEMORY;

    pEnum->m_dwIndex = m_dwIndex;
    *ppenum = pEnum;
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////

MShellItemArray::MShellItemArray() :
    m_nRefCount(1)
{
}

MShellItemArray::~MShellItemArray()
{
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        m_items[i]->Release();
    }
    m_items.clear();
}

/*static*/ MShellItemArray *
MShellItemArray::CreateInstance()
{
    return new(std::nothrow) MShellItemArray();
}

void MShellItemArray::AddItem(IShellItem *pItem)
{
    if (pItem)
    {
        m_items.push_back(pItem);
        pItem->AddRef();
    }
}

STDMETHODIMP MShellItemArray::QueryInterface(REFIID riid, void **ppvObject)
{
    if (riid == IID_IUnknown || riid == IID_IShellItemArray)
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

    for (size_t i = 0; i < m_items.size(); ++i)
    {
        hr = m_items[i]->GetAttributes(sfgaoMask, &attr);
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

    *pdwNumItems = DWORD(m_items.size());
    return S_OK;
}

STDMETHODIMP MShellItemArray::GetItemAt(DWORD dwIndex, IShellItem **ppsi)
{
    if (!ppsi)
        return E_INVALIDARG;

    if (dwIndex < m_items.size())
    {
        *ppsi = m_items[dwIndex];
        m_items[dwIndex]->AddRef();
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
