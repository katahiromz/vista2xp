/* FileDialog_inl.hpp --- v2xol.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */

/*static*/ THIS_CLASS *THIS_CLASS::CreateInstance()
{
    return new(std::nothrow) THIS_CLASS();
}

THIS_CLASS::~THIS_CLASS()
{
    if (m_pidlSelected)
    {
        CoTaskMemFree(m_pidlSelected);
        m_pidlSelected = NULL;
    }
    if (m_pidlDefFolder)
    {
        CoTaskMemFree(m_pidlDefFolder);
        m_pidlDefFolder = NULL;
    }
    if (m_pszzFiles)
    {
        CoTaskMemFree(m_pszzFiles);
        m_pszzFiles = NULL;
    }
    if (m_pszTitle)
    {
        CoTaskMemFree(m_pszTitle);
        m_pszTitle = NULL;
    }
    if (m_pszzFilter)
    {
        CoTaskMemFree(m_pszzFilter);
        m_pszzFilter = NULL;
    }
    if (m_pEvents)
    {
        m_pEvents->Release();
        m_pEvents = NULL;
    }
}

/*static*/ UINT_PTR APIENTRY
THIS_CLASS::OFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    OPENFILENAMEW *pOFN = NULL;
    THIS_CLASS *pFD = NULL;
    OFNOTIFYW *pNotify = NULL;
    IFileDialogEvents *pEvents = NULL;

    switch (uiMsg)
    {
    case WM_INITDIALOG:
        SetWindowLongPtr(hdlg, GWLP_USERDATA, (LONG_PTR)lParam);
        pOFN = (OPENFILENAMEW *)lParam;
        pFD = (THIS_CLASS *)pOFN->lCustData;
        if (pFD)
            pFD->m_hwnd = hdlg;
        break;

    case WM_NOTIFY:
        pOFN = (OPENFILENAMEW *)GetWindowLongPtr(hdlg, GWLP_USERDATA);
        pFD = (THIS_CLASS *)pOFN->lCustData;
        pNotify = (OFNOTIFYW *)lParam;
        pEvents = (IFileDialogEvents *)pFD->m_pEvents;
        if (!pEvents)
            break;

        switch (pNotify->hdr.code)
        {
        case CDN_FILEOK:
            if (!pFD->VerifyFileType() || pEvents->OnFileOk(pFD) == S_FALSE)
            {
                // cancel
                SetWindowLongPtr(hdlg, DWLP_MSGRESULT, TRUE);
                return TRUE;
            }
            break;

        case CDN_FOLDERCHANGE:
            pEvents->OnFolderChange(pFD);
            break;

        case CDN_HELP:
            break;

        case CDN_INITDONE:
            break;

        case CDN_SELCHANGE:
            pEvents->OnSelectionChange(pFD);
            break;

        case CDN_SHAREVIOLATION:
            if (pNotify->pszFile)
            {
                LPITEMIDLIST pidl = ILCreateFromPathW(pNotify->pszFile);
                IShellItem *psi = NULL;
                SHCreateShellItemForXP0(NULL, NULL, pidl, &psi);
                CoTaskMemFree(pidl);
                if (psi)
                {
                    FDE_SHAREVIOLATION_RESPONSE res = FDESVR_DEFAULT;
                    if (S_OK == pEvents->OnShareViolation(pFD, psi, &res))
                    {
                        switch (res)
                        {
                        case FDESVR_DEFAULT:
                            break;

                        case FDESVR_ACCEPT:
                            SetWindowLongPtr(hdlg, DWLP_MSGRESULT, OFN_SHARENOWARN);
                            psi->Release();
                            return TRUE;

                        case FDESVR_REFUSE:
                            psi->Release();
                            return FALSE;
                        }
                    }
                    psi->Release();
                }
            }
            break;

        case CDN_TYPECHANGE:
            pEvents->OnTypeChange(pFD);
            break;
        }
        break;
    }

    return 0;
}

/*static*/ int CALLBACK
THIS_CLASS::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    THIS_CLASS *pFD = (THIS_CLASS *)lpData;
    IFileDialogEvents *pEvents = NULL;
    if (pFD)
        pEvents = pFD->m_pEvents;

    switch (uMsg)
    {
    case BFFM_INITIALIZED:
        if (pFD)
        {
            pFD->m_hwnd = hwnd;
            ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)pFD->m_szFile);
        }
        break;

    case BFFM_SELCHANGED:
        if (pFD->m_pidlSelected && pFD->m_pidlSelected != (LPITEMIDLIST)lParam)
        {
            CoTaskMemFree(pFD->m_pidlSelected);
            pFD->m_pidlSelected = NULL;
        }
        pFD->m_pidlSelected = ILClone((LPITEMIDLIST)lParam);
        if (pEvents)
            pEvents->OnFolderChange(pFD);
        break;

    case BFFM_VALIDATEFAILED:
        break;
    }

    return 0;
}

BOOL THIS_CLASS::IsFolderDialog() const
{
    return !!(m_options & FOS_PICKFOLDERS);
}

BOOL THIS_CLASS::UpdateFlags()
{
    // TODO: FOS_SUPPORTSTREAMABLEITEMS,
    //       FOS_ALLNONSTORAGEITEMS, FOS_HIDEPINNEDPLACES,
    //       FOS_FORCEPREVIEWPANEON, FOS_SUPPORTSTREAMABLEITEMS

    if ((m_options & FOS_ALLNONSTORAGEITEMS) && (m_options & FOS_FORCEFILESYSTEM))
        return FALSE;

    if (m_options & FOS_OVERWRITEPROMPT)
        m_ofn.Flags |= OFN_OVERWRITEPROMPT;
    else
        m_ofn.Flags &= ~OFN_OVERWRITEPROMPT;

    if (m_options & FOS_NOCHANGEDIR)
        m_ofn.Flags |= OFN_NOCHANGEDIR;
    else
        m_ofn.Flags &= ~OFN_NOCHANGEDIR;

    if (m_options & FOS_FORCEFILESYSTEM)
        m_bi.ulFlags |= BIF_RETURNONLYFSDIRS;
    else
        m_bi.ulFlags &= ~BIF_RETURNONLYFSDIRS;

    if (m_options & FOS_NOVALIDATE)
    {
        m_ofn.Flags |= OFN_NOVALIDATE;
        m_bi.ulFlags &= ~BIF_VALIDATE;
    }
    else
    {
        m_ofn.Flags &= ~OFN_NOVALIDATE;
        m_bi.ulFlags |= BIF_VALIDATE;
    }

    if (m_options & FOS_ALLOWMULTISELECT)
        m_ofn.Flags |= OFN_ALLOWMULTISELECT;
    else
        m_ofn.Flags &= ~OFN_ALLOWMULTISELECT;

    if (m_options & FOS_PATHMUSTEXIST)
        m_ofn.Flags |= OFN_PATHMUSTEXIST;
    else
        m_ofn.Flags &= ~OFN_PATHMUSTEXIST;

    if (m_options & FOS_FILEMUSTEXIST)
        m_ofn.Flags |= OFN_FILEMUSTEXIST;
    else
        m_ofn.Flags &= ~OFN_FILEMUSTEXIST;

    if (m_options & FOS_CREATEPROMPT)
        m_ofn.Flags |= OFN_CREATEPROMPT;
    else
        m_ofn.Flags &= ~OFN_CREATEPROMPT;

    if (m_options & FOS_SHAREAWARE)
        m_ofn.Flags |= OFN_SHAREAWARE;
    else
        m_ofn.Flags &= ~OFN_SHAREAWARE;

    if (m_options & FOS_NOREADONLYRETURN)
        m_ofn.Flags |= OFN_NOREADONLYRETURN;
    else
        m_ofn.Flags &= ~OFN_NOREADONLYRETURN;

    if (m_options & FOS_NOTESTFILECREATE)
        m_ofn.Flags |= OFN_NOTESTFILECREATE;
    else
        m_ofn.Flags &= ~OFN_NOTESTFILECREATE;

#ifndef OFN_EX_NOPLACESBAR
    #define OFN_EX_NOPLACESBAR 0x1
#endif
#ifndef OFN_FORCESHOWHIDDEN
    #define OFN_FORCESHOWHIDDEN 0x10000000
#endif

    if (m_options & FOS_HIDEMRUPLACES)
        m_ofn.FlagsEx |= OFN_EX_NOPLACESBAR;
    else
        m_ofn.FlagsEx  &= ~OFN_EX_NOPLACESBAR;

    if (m_options & FOS_NODEREFERENCELINKS)
        m_ofn.Flags |= OFN_NODEREFERENCELINKS;
    else
        m_ofn.Flags &= ~OFN_NODEREFERENCELINKS;

    if (m_options & FOS_DONTADDTORECENT)
        m_ofn.Flags |= OFN_DONTADDTORECENT;
    else
        m_ofn.Flags &= ~OFN_DONTADDTORECENT;

    if (m_options & FOS_FORCESHOWHIDDEN)
        m_ofn.Flags |= OFN_FORCESHOWHIDDEN;
    else
        m_ofn.Flags &= ~OFN_FORCESHOWHIDDEN;

    return TRUE;
}

LPITEMIDLIST THIS_CLASS::GetFolderIDList()
{
    if (IsFolderDialog())
    {
        return ILClone(m_pidlSelected);
    }
    else
    {
        if (::IsWindow(m_hwnd))
        {
            SIZE_T size = (SIZE_T)::SendMessage(::GetParent(m_hwnd), CDM_GETFOLDERIDLIST, 0, 0);
            LPITEMIDLIST pidl = (LPITEMIDLIST)CoTaskMemAlloc(size);
            if (!pidl)
                return NULL;

            ::SendMessage(::GetParent(m_hwnd), CDM_GETFOLDERIDLIST, size, (LPARAM)pidl);
            return pidl;
        }
        else
        {
            if (m_szFile[0] == 0)
                return NULL;

            if (PathIsDirectoryW(m_szFile))
                return ILCreateFromPathW(m_szFile);

            WCHAR szPath[MAX_PATH];
            StringCbCopyW(szPath, sizeof(szPath), m_szFile);
            LPWSTR pch = PathFindFileNameW(szPath);
            if (pch)
                *pch = 0;
            PathRemoveBackslashW(szPath);
            return ILCreateFromPathW(szPath);
        }
    }
}

BOOL THIS_CLASS::VerifyFileType()
{
    if (!(m_options & FOS_STRICTFILETYPES))
        return TRUE;

    if (m_options & FOS_ALLOWMULTISELECT)
        return TRUE;    // FIXME

    if (!*m_szFile)
        return TRUE;

    LPWSTR pchExt1 = PathFindExtension(m_szFile);
    if (pchExt1 && *pchExt1 == L'.')
        ++pchExt1;

    if (lstrcmpiW(m_szDefExt, pchExt1) == 0)
        return TRUE;

    LPWSTR pch = m_pszzFilter;
    if (!pch || *pch == 0)
        return TRUE;

    INT nIndex;
    if (m_ofn.nFilterIndex == 0)
        nIndex = 0;
    else
        nIndex = m_ofn.nFilterIndex - 1;

    // skip
    for (INT i = 0; i < nIndex; ++i)
    {
        pch += lstrlenW(pch) + 1;
        pch += lstrlenW(pch) + 1;
    }
    pch += lstrlenW(pch);

    // check extensions
    LPWSTR pchNext, pchExt2;
    WCHAR szExt[32];
    while (pch && *pch)
    {
        pchNext = wcschr(pch, L';');
        if (pchNext == NULL)
            pchNext = pch + lstrlenW(pch);

        if (pchNext - pch < ARRAYSIZE(szExt))
        {
            ZeroMemory(szExt, sizeof(szExt));
            StringCbCopyW(szExt, sizeof(szExt), pch);
            szExt[pchNext - pch] = 0;

            pchExt2 = szExt;
            while (*pchExt2 == L'*' || *pchExt2 == L'?' || *pchExt2 == L'.')
                ++pchExt2;

            if (*pchExt2 == 0)
                return TRUE;

            if (lstrcmpiW(pchExt1, pchExt2) == 0)
                return TRUE;
        }

        pch = pchNext;
    }

    return FALSE;
}

// IUnknown interface

STDMETHODIMP_(ULONG) THIS_CLASS::AddRef()
{
    m_nRefCount++;
    return m_nRefCount;
}

STDMETHODIMP_(ULONG) THIS_CLASS::Release()
{
    --m_nRefCount;
    if (m_nRefCount != 0)
        return m_nRefCount;

    delete this;
    return 0;
}

// IModalWindow interface

STDMETHODIMP THIS_CLASS::Show(HWND hwndOwner)
{
    if (!UpdateFlags())
        return E_INVALIDARG;

    HRESULT hr = ERROR_CANCELLED;
    if (IsFolderDialog())
    {
        m_bi.hwndOwner = hwndOwner;
        m_pidlSelected = SHBrowseForFolder(&m_bi);
        if (m_pidlSelected)
        {
            hr = S_OK;
        }
    }
    else
    {
        m_ofn.hwndOwner = hwndOwner;
        if (m_options & FOS_ALLOWMULTISELECT)
        {
            if (m_pszzFiles)
            {
                CoTaskMemFree(m_pszzFiles);
                m_pszzFiles = NULL;
            }

            m_ofn.nMaxFile = 16 * 1024;
            m_pszzFiles = (LPWSTR)CoTaskMemAlloc(m_ofn.nMaxFile * sizeof(WCHAR));
            if (!m_pszzFiles)
                return E_OUTOFMEMORY;

            StringCchCopyW(m_pszzFiles, m_ofn.nMaxFile, m_szFile);
            m_ofn.lpstrFile = m_pszzFiles;
        }
        else
        {
            m_ofn.lpstrFile = m_szFile;
            m_ofn.nMaxFile = ARRAYSIZE(m_szFile);
        }

        WCHAR szInitDir[MAX_PATH];
        if (m_pidlDefFolder)
        {
            SHGetPathFromIDListW(m_pidlDefFolder, szInitDir);
            m_ofn.lpstrInitialDir = szInitDir;
        }
        else
        {
            m_ofn.lpstrInitialDir = NULL;
        }

        m_ofn.lpstrFilter = m_pszzFilter;

        if (m_bDoSave)
        {
            if (::GetSaveFileNameW(&m_ofn))
            {
                hr = S_OK;
            }
        }
        else
        {
            if (::GetOpenFileNameW(&m_ofn))
            {
                hr = S_OK;
            }
        }
    }

    m_hwnd = NULL;
    return hr;
}

// IFileDialog interface

STDMETHODIMP THIS_CLASS::SetFileTypes(
    UINT cFileTypes,
    const COMDLG_FILTERSPEC *rgFilterSpec)
{
    if (!rgFilterSpec)
        return E_INVALIDARG;

    if (m_pszzFilter)
    {
        CoTaskMemFree(m_pszzFilter);
        m_pszzFilter = NULL;
    }

    if (cFileTypes == 0)
        return S_OK;

    // get length of filter
    UINT cchFilterMax = 0;
    for (UINT i = 0; i < cFileTypes; ++i)
    {
        cchFilterMax += (UINT)lstrlenW(rgFilterSpec[i].pszName) + 1;
        cchFilterMax += (UINT)lstrlenW(rgFilterSpec[i].pszSpec) + 1;
    }
    ++cchFilterMax;

    ULONG cb = cchFilterMax * sizeof(WCHAR);
    m_pszzFilter = (LPWSTR)CoTaskMemAlloc(cb);
    if (!m_pszzFilter)
        return E_OUTOFMEMORY;

    // build filter
    m_pszzFilter[0] = 0;
    for (UINT i = 0; i < cFileTypes; ++i)
    {
        StringCchCatW(m_pszzFilter, cchFilterMax, rgFilterSpec[i].pszName);
        StringCchCatW(m_pszzFilter, cchFilterMax, L"|");
        StringCchCatW(m_pszzFilter, cchFilterMax, rgFilterSpec[i].pszSpec);
        StringCchCatW(m_pszzFilter, cchFilterMax, L"|");
    }

    for (LPWSTR pch = m_pszzFilter; *pch; ++pch)
    {
        if (*pch == L'|')
            *pch = 0;
    }

    return S_OK;
}

STDMETHODIMP THIS_CLASS::SetFileTypeIndex(UINT iFileType)
{
    m_ofn.nFilterIndex = iFileType;
    return S_OK;
}

STDMETHODIMP THIS_CLASS::GetFileTypeIndex(UINT *piFileType)
{
    if (!piFileType)
        return E_INVALIDARG;

    *piFileType = m_ofn.nFilterIndex;
    return S_OK;
}

STDMETHODIMP THIS_CLASS::Advise(
    IFileDialogEvents *pfde,
    DWORD *pdwCookie)
{
    if (!pfde || !pdwCookie)
        return E_INVALIDARG;

    if (m_pEvents)
        return E_FAIL;

    m_dwCookie = 0xDEADFACE;
    *pdwCookie = m_dwCookie;

    m_pEvents = pfde;
    m_pEvents->AddRef();

    return S_OK;
}

STDMETHODIMP THIS_CLASS::Unadvise(DWORD dwCookie)
{
    if (m_dwCookie != dwCookie)
        return E_INVALIDARG;

    if (m_pEvents)
    {
        m_pEvents->Release();
        m_pEvents = NULL;
        return S_OK;
    }

    return E_FAIL;
}

STDMETHODIMP THIS_CLASS::SetOptions(FILEOPENDIALOGOPTIONS fos)
{
    m_options = fos;
    return S_OK;
}

STDMETHODIMP THIS_CLASS::GetOptions(FILEOPENDIALOGOPTIONS *pfos)
{
    if (!pfos)
        return E_INVALIDARG;
    *pfos = m_options;
    return S_OK;
}

STDMETHODIMP THIS_CLASS::SetDefaultFolder(IShellItem *psi)
{
    if (!psi)
        return E_POINTER;

    if (m_pidlDefFolder)
    {
        CoTaskMemFree(m_pidlDefFolder);
        m_pidlDefFolder = NULL;
    }

    return SHGetIDListFromObjectForXP(psi, &m_pidlDefFolder);
}

STDMETHODIMP THIS_CLASS::SetFolder(IShellItem *psi)
{
    if (!psi)
        return E_INVALIDARG;

    LPITEMIDLIST pidl;
    SHGetIDListFromObjectForXP(psi, &pidl);
    if (!pidl)
        return E_OUTOFMEMORY;

    if (IsFolderDialog())
    {
        if (::IsWindow(m_hwnd))
        {
            ::SendMessage(m_hwnd, BFFM_SETSELECTION, FALSE, (LPARAM)pidl);
        }
        else
        {
            if (m_pidlSelected)
            {
                CoTaskMemFree(m_pidlSelected);
                m_pidlSelected = NULL;
            }
            m_pidlSelected = ILClone(pidl);
        }
    }
    else
    {
        SHGetPathFromIDListW(pidl, m_szFile);
    }

    CoTaskMemFree(pidl);
    return S_OK;
}

STDMETHODIMP THIS_CLASS::GetFolder(IShellItem **ppsi)
{
    if (!ppsi)
        return E_INVALIDARG;

    *ppsi = NULL;

    LPITEMIDLIST pidl = GetFolderIDList();
    if (pidl)
    {
        HRESULT hr = SHCreateShellItemForXP0(NULL, NULL, pidl, ppsi);
        CoTaskMemFree(pidl);
        return hr;
    }
    return E_FAIL;
}

STDMETHODIMP THIS_CLASS::GetCurrentSelection(IShellItem **ppsi)
{
    if (!ppsi)
        return E_INVALIDARG;
    if (!::IsWindow(m_hwnd))
        return E_FAIL;

    HRESULT hr;
    if (IsFolderDialog())
    {
        hr = SHCreateShellItemForXP0(NULL, NULL, m_pidlSelected, ppsi);
    }
    else
    {
        SIZE_T size = (SIZE_T)SendMessage(GetParent(m_hwnd), CDM_GETFOLDERIDLIST, 0, 0);
        LPITEMIDLIST pidl = (LPITEMIDLIST)CoTaskMemAlloc(size);
        if (!pidl)
            return E_OUTOFMEMORY;

        ::SendMessage(GetParent(m_hwnd), CDM_GETFOLDERIDLIST, size, (LPARAM)pidl);
        hr = SHCreateShellItemForXP0(NULL, NULL, pidl, ppsi);
        CoTaskMemFree(pidl);
    }

    return hr;
}

STDMETHODIMP THIS_CLASS::SetFileName(LPCWSTR pszName)
{
    if (!pszName)
        return E_POINTER;

    if (IsFolderDialog())
    {
        if (::IsWindow(m_hwnd))
        {
            ::SendMessage(m_hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)pszName);
            return S_OK;
        }
        else
        {
            if (m_pidlSelected)
            {
                CoTaskMemFree(m_pidlSelected);
                m_pidlSelected = NULL;
            }

            m_pidlSelected = ILCreateFromPathW(pszName);
            if (!m_pidlSelected)
                return E_OUTOFMEMORY;
        }
    }

    StringCbCopy(m_szFile, sizeof(m_szFile), pszName);
    return S_OK;
}

STDMETHODIMP THIS_CLASS::GetFileName(LPWSTR *pszName)
{
    LPWSTR psz;
    ULONG cb;
    WCHAR szPath[MAX_PATH];

    if (!pszName)
        return E_INVALIDARG;

    if (IsFolderDialog())
    {
        if (m_pidlSelected)
        {
            SHGetPathFromIDListW(m_pidlSelected, szPath);
            cb = lstrlenW(szPath + 1) * sizeof(WCHAR);
            psz = (LPWSTR)CoTaskMemAlloc(cb);
            if (psz)
            {
                CopyMemory(psz, szPath, cb);
                *pszName = psz;
                return S_OK;
            }
        }
    }
    else
    {
        if (m_szFile[0])
        {
            cb = lstrlenW(m_szFile + 1) * sizeof(WCHAR);
            psz = (LPWSTR)CoTaskMemAlloc(cb);
            if (psz)
            {
                CopyMemory(psz, m_szFile, cb);
                *pszName = psz;
                return S_OK;
            }
        }
    }

    *pszName = NULL;
    return E_FAIL;
}

STDMETHODIMP THIS_CLASS::SetTitle(LPCWSTR pszTitle)
{
    if (!pszTitle)
        return E_POINTER;

    ULONG cb = (lstrlenW(pszTitle) + 1) * sizeof(WCHAR);
    m_pszTitle = (LPWSTR)CoTaskMemAlloc(cb);
    if (m_pszTitle)
    {
        CopyMemory(m_pszTitle, pszTitle, cb);
        return S_OK;
    }
    return E_FAIL;
}

STDMETHODIMP THIS_CLASS::SetOkButtonLabel(LPCWSTR pszText)
{
    if (::IsWindow(m_hwnd))
    {
        if (IsFolderDialog())
        {
            ::SendMessage(m_hwnd, BFFM_SETOKTEXT, 0, (LPARAM)pszText);
        }
        else
        {
            ::SendMessage(::GetParent(m_hwnd), CDM_SETCONTROLTEXT, IDOK, (LPARAM)pszText);
        }
        return S_OK;
    }

    return E_FAIL;
}

STDMETHODIMP THIS_CLASS::SetFileNameLabel(LPCWSTR pszLabel)
{
    if (::IsWindow(m_hwnd))
    {
        ::SendMessage(m_hwnd, CDM_SETCONTROLTEXT, stc3, (LPARAM)pszLabel);
    }
    return E_FAIL;
}

STDMETHODIMP THIS_CLASS::GetResult(IShellItem **ppsi)
{
    HRESULT hr;
    LPITEMIDLIST pidl;

    if (IsFolderDialog())
    {
        return SHCreateShellItemForXP0(NULL, NULL, m_pidlSelected, ppsi);
    }
    else
    {
        if (m_pszzFiles)
        {
            pidl = ILCreateFromPathW(m_pszzFiles);
        }
        else
        {
            pidl = ILCreateFromPathW(m_szFile);
        }
        hr = SHCreateShellItemForXP0(NULL, NULL, pidl, ppsi);
        CoTaskMemFree(pidl);
        return hr;
    }
}

STDMETHODIMP THIS_CLASS::AddPlace(IShellItem *psi, FDAP fdap)
{
    return E_NOTIMPL;
}

STDMETHODIMP THIS_CLASS::SetDefaultExtension(LPCWSTR pszDefaultExtension)
{
    if (IsFolderDialog())
        return E_INVALIDARG;

    if (pszDefaultExtension && *pszDefaultExtension)
    {
        if (*pszDefaultExtension == L'.')
            ++pszDefaultExtension;

        if (*pszDefaultExtension)
        {
            StringCbCopy(m_szDefExt, sizeof(m_szDefExt), pszDefaultExtension);
            m_ofn.lpstrDefExt = m_szDefExt;

            if (::IsWindow(m_hwnd))
            {
                ::SendMessage(m_hwnd, CDM_SETDEFEXT, 0, (LPARAM)pszDefaultExtension);
            }
            return S_OK;
        }
    }

    m_ofn.lpstrDefExt = NULL;
    return S_OK;
}

STDMETHODIMP THIS_CLASS::Close(HRESULT hr)
{
    if (::IsWindow(m_hwnd))
    {
        ::PostMessage(m_hwnd, WM_COMMAND, IDABORT, 0);
        return hr;
    }
    return E_FAIL;
}

STDMETHODIMP THIS_CLASS::SetClientGuid(REFGUID guid)
{
    return E_NOTIMPL;
}

STDMETHODIMP THIS_CLASS::ClearClientData()
{
    return E_NOTIMPL;
}

STDMETHODIMP THIS_CLASS::SetFilter(IShellItemFilter *pFilter)
{
    return E_NOTIMPL;
}
