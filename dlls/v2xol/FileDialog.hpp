/* FileDialog.hpp --- v2xol.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#ifndef FILE_DIALOG_HPP_
#define FILE_DIALOG_HPP_

#include <windows.h>
#include <objbase.h>
#include <shobjidl.h>

interface IFileDialog;
    interface IFileOpenDialog;
    interface IFileSaveDialog;
interface IFileDialogEvents;

interface IShellItemFilter : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE IncludeItem(
        IShellItem *psi) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetEnumFlagsForItem(
        IShellItem *psi,
        SHCONTF *pgrfFlags) = 0;
};

typedef enum FDE_OVERWRITE_RESPONSE {
    FDEOR_DEFAULT = 0,
    FDEOR_ACCEPT = 1,
    FDEOR_REFUSE = 2
} FDE_OVERWRITE_RESPONSE;

typedef enum FDE_SHAREVIOLATION_RESPONSE {
    FDESVR_DEFAULT = 0,
    FDESVR_ACCEPT = 1,
    FDESVR_REFUSE = 2
} FDE_SHAREVIOLATION_RESPONSE;

typedef enum FDAP {
    FDAP_BOTTOM = 0,
    FDAP_TOP = 1
} FDAP;

interface IFileDialogEvents : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE OnFileOk(
        IFileDialog *pfd) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnFolderChanging(
        IFileDialog *pfd,
        IShellItem *psiFolder) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnFolderChange(
        IFileDialog *pfd) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnSelectionChange(
        IFileDialog *pfd) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnShareViolation(
        IFileDialog *pfd,
        IShellItem *psi,
        FDE_SHAREVIOLATION_RESPONSE *pResponse) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnTypeChange(
        IFileDialog *pfd) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnOverwrite(
        IFileDialog *pfd,
        IShellItem *psi,
        FDE_OVERWRITE_RESPONSE *pResponse) = 0;
};

enum _FILEOPENDIALOGOPTIONS {
    FOS_OVERWRITEPROMPT = 0x2,
    FOS_STRICTFILETYPES = 0x4,
    FOS_NOCHANGEDIR = 0x8,
    FOS_PICKFOLDERS = 0x20,
    FOS_FORCEFILESYSTEM = 0x40,
    FOS_ALLNONSTORAGEITEMS = 0x80,
    FOS_NOVALIDATE = 0x100,
    FOS_ALLOWMULTISELECT = 0x200,
    FOS_PATHMUSTEXIST = 0x800,
    FOS_FILEMUSTEXIST = 0x1000,
    FOS_CREATEPROMPT = 0x2000,
    FOS_SHAREAWARE = 0x4000,
    FOS_NOREADONLYRETURN = 0x8000,
    FOS_NOTESTFILECREATE = 0x10000,
    FOS_HIDEMRUPLACES = 0x20000,
    FOS_HIDEPINNEDPLACES = 0x40000,
    FOS_NODEREFERENCELINKS = 0x100000,
    FOS_DONTADDTORECENT = 0x2000000,
    FOS_FORCESHOWHIDDEN = 0x10000000,
    FOS_DEFAULTNOMINIMODE = 0x20000000,
    FOS_FORCEPREVIEWPANEON = 0x40000000
};

typedef DWORD FILEOPENDIALOGOPTIONS;

interface IFileDialog : public IModalWindow
{
    virtual HRESULT STDMETHODCALLTYPE SetFileTypes(
        UINT cFileTypes,
        const COMDLG_FILTERSPEC *rgFilterSpec) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFileTypeIndex(
        UINT iFileType) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetFileTypeIndex(
        UINT *piFileType) = 0;

    virtual HRESULT STDMETHODCALLTYPE Advise(
        IFileDialogEvents *pfde,
        DWORD *pdwCookie) = 0;

    virtual HRESULT STDMETHODCALLTYPE Unadvise(
        DWORD dwCookie) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetOptions(
        FILEOPENDIALOGOPTIONS fos) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetOptions(
        FILEOPENDIALOGOPTIONS *pfos) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDefaultFolder(
        IShellItem *psi) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFolder(
        IShellItem *psi) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetFolder(
        IShellItem **ppsi) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetCurrentSelection(
        IShellItem **ppsi) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFileName(
        LPCWSTR pszName) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetFileName(
        LPWSTR *pszName) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetTitle(
        LPCWSTR pszTitle) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetOkButtonLabel(
        LPCWSTR pszText) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFileNameLabel(
        LPCWSTR pszLabel) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetResult(
        IShellItem **ppsi) = 0;

    virtual HRESULT STDMETHODCALLTYPE AddPlace(
        IShellItem *psi,
        FDAP fdap) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDefaultExtension(
        LPCWSTR pszDefaultExtension) = 0;

    virtual HRESULT STDMETHODCALLTYPE Close(
        HRESULT hr) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetClientGuid(
        REFGUID guid) = 0;

    virtual HRESULT STDMETHODCALLTYPE ClearClientData(
        ) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFilter(
        IShellItemFilter *pFilter) = 0;
};

interface IFileOpenDialog : public IFileDialog
{
    virtual HRESULT STDMETHODCALLTYPE GetResults(
        IShellItemArray **ppenum) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetSelectedItems(
        IShellItemArray **ppsai) = 0;
};

interface IFileSaveDialog : public IFileDialog
{
    virtual HRESULT STDMETHODCALLTYPE SetSaveAsItem(
        IShellItem *psi) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetProperties(
        IPropertyStore *pStore) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetCollectedProperties(
        IPropertyDescriptionList *pList,
        BOOL fAppendDefault) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetProperties(
        IPropertyStore **ppStore) = 0;

    virtual HRESULT STDMETHODCALLTYPE ApplyProperties(
        IShellItem *psi,
        IPropertyStore *pStore,
        HWND hwnd,
        IFileOperationProgressSink *pSink) = 0;
};

IFileOpenDialog *createFileOpenDialog(void);
IFileSaveDialog *createFileSaveDialog(void);

#endif  // ndef FILE_DIALOG_HPP_
