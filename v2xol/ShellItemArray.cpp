/* ShellItemArray.cpp --- v2xol.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */

#include "ShellItemArray.hpp"
#include <algorithm> // for std::min

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
    m_dwIndex = std::min(m_dwIndex + celt, m_dwCount - 1);
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
