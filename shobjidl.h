#ifndef SHOBJIDL_H_
#define SHOBJIDL_H_

#include <shlobj.h>

#if defined(STRICT_TYPED_ITEMIDS) && defined(__cplusplus)
typedef ITEMIDLIST_ABSOLUTE *PIDLIST_ABSOLUTE;
typedef const ITEMIDLIST_ABSOLUTE *PCIDLIST_ABSOLUTE;
typedef const ITEMIDLIST_ABSOLUTE *PCUIDLIST_ABSOLUTE;
typedef ITEMIDLIST_RELATIVE *PIDLIST_RELATIVE;
typedef const ITEMIDLIST_RELATIVE *PCIDLIST_RELATIVE;
typedef ITEMIDLIST_RELATIVE *PUIDLIST_RELATIVE;
typedef const ITEMIDLIST_RELATIVE *PCUIDLIST_RELATIVE;
typedef ITEMID_CHILD *PITEMID_CHILD;
typedef const ITEMID_CHILD *PCITEMID_CHILD;
typedef ITEMID_CHILD *PUITEMID_CHILD;
typedef const ITEMID_CHILD *PCUITEMID_CHILD;
typedef const PCUITEMID_CHILD *PCUITEMID_CHILD_ARRAY;
typedef const PCUIDLIST_RELATIVE *PCUIDLIST_RELATIVE_ARRAY;
typedef const PCIDLIST_ABSOLUTE *PCIDLIST_ABSOLUTE_ARRAY;
typedef const PCUIDLIST_ABSOLUTE *PCUIDLIST_ABSOLUTE_ARRAY;
#else
#define PIDLIST_ABSOLUTE LPITEMIDLIST
#define PCIDLIST_ABSOLUTE LPCITEMIDLIST
#define PCUIDLIST_ABSOLUTE LPCITEMIDLIST
#define PIDLIST_RELATIVE LPITEMIDLIST
#define PCIDLIST_RELATIVE LPCITEMIDLIST
#define PUIDLIST_RELATIVE LPITEMIDLIST
#define PCUIDLIST_RELATIVE LPCITEMIDLIST
#define PITEMID_CHILD LPITEMIDLIST
#define PCITEMID_CHILD LPCITEMIDLIST
#define PUITEMID_CHILD LPITEMIDLIST
#define PCUITEMID_CHILD LPCITEMIDLIST
#define PCUITEMID_CHILD_ARRAY LPCITEMIDLIST *
#define PCUIDLIST_RELATIVE_ARRAY LPCITEMIDLIST *
#define PCIDLIST_ABSOLUTE_ARRAY LPCITEMIDLIST *
#define PCUIDLIST_ABSOLUTE_ARRAY LPCITEMIDLIST *
#endif

#ifndef SFGAO_CANCOPY
    #define SFGAO_CANCOPY DROPEFFECT_COPY
#endif
#ifndef SFGAO_CANMOVE
    #define SFGAO_CANMOVE DROPEFFECT_MOVE
#endif
#ifndef SFGAO_CANLINK
    #define SFGAO_CANLINK DROPEFFECT_LINK
#endif
#ifndef SFGAO_STORAGE
    #define SFGAO_STORAGE __MSABI_LONG(0x8)
#endif
#ifndef SFGAO_CANRENAME
    #define SFGAO_CANRENAME __MSABI_LONG(0x10)
#endif
#ifndef SFGAO_CANDELETE
    #define SFGAO_CANDELETE __MSABI_LONG(0x20)
#endif
#ifndef SFGAO_HASPROPSHEET
    #define SFGAO_HASPROPSHEET __MSABI_LONG(0x40)
#endif
#ifndef SFGAO_DROPTARGET
    #define SFGAO_DROPTARGET __MSABI_LONG(0x100)
#endif
#ifndef SFGAO_CAPABILITYMASK
    #define SFGAO_CAPABILITYMASK __MSABI_LONG(0x177)
#endif
#ifndef SFGAO_SYSTEM
    #define SFGAO_SYSTEM __MSABI_LONG(0x1000)
#endif
#ifndef SFGAO_ENCRYPTED
    #define SFGAO_ENCRYPTED __MSABI_LONG(0x2000)
#endif
#ifndef SFGAO_ISSLOW
    #define SFGAO_ISSLOW __MSABI_LONG(0x4000)
#endif
#ifndef SFGAO_GHOSTED
    #define SFGAO_GHOSTED __MSABI_LONG(0x8000)
#endif
#ifndef SFGAO_LINK
    #define SFGAO_LINK __MSABI_LONG(0x10000)
#endif
#ifndef SFGAO_SHARE
    #define SFGAO_SHARE __MSABI_LONG(0x20000)
#endif
#ifndef SFGAO_READONLY
    #define SFGAO_READONLY __MSABI_LONG(0x40000)
#endif
#ifndef SFGAO_HIDDEN
    #define SFGAO_HIDDEN __MSABI_LONG(0x80000)
#endif
#ifndef SFGAO_DISPLAYATTRMASK
    #define SFGAO_DISPLAYATTRMASK __MSABI_LONG(0xfc000)
#endif
#ifndef SFGAO_FILESYSANCESTOR
    #define SFGAO_FILESYSANCESTOR __MSABI_LONG(0x10000000)
#endif
#ifndef SFGAO_FOLDER
    #define SFGAO_FOLDER __MSABI_LONG(0x20000000)
#endif
#ifndef SFGAO_FILESYSTEM
    #define SFGAO_FILESYSTEM __MSABI_LONG(0x40000000)
#endif
#ifndef SFGAO_HASSUBFOLDER
    #define SFGAO_HASSUBFOLDER __MSABI_LONG(0x80000000)
#endif
#ifndef SFGAO_CONTENTSMASK
    #define SFGAO_CONTENTSMASK __MSABI_LONG(0x80000000)
#endif
#ifndef SFGAO_VALIDATE
    #define SFGAO_VALIDATE __MSABI_LONG(0x1000000)
#endif
#ifndef SFGAO_REMOVABLE
    #define SFGAO_REMOVABLE __MSABI_LONG(0x2000000)
#endif
#ifndef SFGAO_COMPRESSED
    #define SFGAO_COMPRESSED __MSABI_LONG(0x4000000)
#endif
#ifndef SFGAO_BROWSABLE
    #define SFGAO_BROWSABLE __MSABI_LONG(0x8000000)
#endif
#ifndef SFGAO_NONENUMERATED
    #define SFGAO_NONENUMERATED __MSABI_LONG(0x100000)
#endif
#ifndef SFGAO_NEWCONTENT
    #define SFGAO_NEWCONTENT __MSABI_LONG(0x200000)
#endif
#ifndef SFGAO_CANMONIKER
    #define SFGAO_CANMONIKER __MSABI_LONG(0x400000)
#endif
#ifndef SFGAO_HASSTORAGE
    #define SFGAO_HASSTORAGE __MSABI_LONG(0x400000)
#endif
#ifndef SFGAO_STREAM
    #define SFGAO_STREAM __MSABI_LONG(0x400000)
#endif
#ifndef SFGAO_STORAGEANCESTOR
    #define SFGAO_STORAGEANCESTOR __MSABI_LONG(0x00800000)
#endif
#ifndef SFGAO_STORAGECAPMASK
    #define SFGAO_STORAGECAPMASK __MSABI_LONG(0x70c50008)
#endif
#ifndef SFGAO_PKEYSFGAOMASK
    #define SFGAO_PKEYSFGAOMASK __MSABI_LONG(0x81044000)
#endif

typedef ULONG SFGAOF;

#ifndef GPS_DEFINED
    #define GPS_DEFINED
    typedef enum GETPROPERTYSTOREFLAGS {
        GPS_DEFAULT = 0x0,
        GPS_HANDLERPROPERTIESONLY = 0x1,
        GPS_READWRITE = 0x2,
        GPS_TEMPORARY = 0x4,
        GPS_FASTPROPERTIESONLY = 0x8,
        GPS_OPENSLOWITEM = 0x10,
        GPS_DELAYCREATION = 0x20,
        GPS_BESTEFFORT = 0x40,
        GPS_NO_OPLOCK = 0x80,
        GPS_PREFERQUERYPROPERTIES = 0x100,
        GPS_MASK_VALID = 0x1ff
    } GETPROPERTYSTOREFLAGS;
#endif

#ifndef REFPROPERTYKEY
    typedef struct _tagpropertykey {
      GUID fmtid;
      DWORD pid;
    } PROPERTYKEY;

    #ifndef REFPROPERTYKEY
    #ifdef __cplusplus
    #define REFPROPERTYKEY const PROPERTYKEY &
    #else /*!__cplusplus*/
    #define REFPROPERTYKEY const PROPERTYKEY * __MIDL_CONST
    #endif
    #endif
#endif

#ifndef __IShellItem_INTERFACE_DEFINED__
#define __IShellItem_INTERFACE_DEFINED__
typedef enum _SIGDN {
    SIGDN_NORMALDISPLAY = 0x0,
    SIGDN_PARENTRELATIVEPARSING = (int)0x80018001,
    SIGDN_DESKTOPABSOLUTEPARSING = (int)0x80028000,
    SIGDN_PARENTRELATIVEEDITING = (int)0x80031001,
    SIGDN_DESKTOPABSOLUTEEDITING = (int)0x8004c000,
    SIGDN_FILESYSPATH = (int)0x80058000,
    SIGDN_URL = (int)0x80068000,
    SIGDN_PARENTRELATIVEFORADDRESSBAR = (int)0x8007c001,
    SIGDN_PARENTRELATIVE = (int)0x80080001,
    SIGDN_PARENTRELATIVEFORUI = (int)0x80094001
} SIGDN;

enum _SICHINTF {
    SICHINT_DISPLAY = 0x0,
    SICHINT_ALLFIELDS = (int)0x80000000,
    SICHINT_CANONICAL = 0x10000000,
    SICHINT_TEST_FILESYSPATH_IF_NOT_EQUAL = 0x20000000
};
typedef DWORD SICHINTF;

interface IShellItem : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE BindToHandler(
        IBindCtx *pbc,
        REFGUID bhid,
        REFIID riid,
        void **ppv) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetParent(
        IShellItem **ppsi) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetDisplayName(
        SIGDN sigdnName,
        LPWSTR *ppszName) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetAttributes(
        SFGAOF sfgaoMask,
        SFGAOF *psfgaoAttribs) = 0;

    virtual HRESULT STDMETHODCALLTYPE Compare(
        IShellItem *psi,
        SICHINTF hint,
        int *piOrder) = 0;
};
#endif

/*****************************************************************************
 * IPersistIDList interface
 */
#ifndef __IPersistIDList_INTERFACE_DEFINED__
#define __IPersistIDList_INTERFACE_DEFINED__
interface IPersistIDList : public IPersist
{
    virtual HRESULT STDMETHODCALLTYPE SetIDList(
        PCIDLIST_ABSOLUTE pidl) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetIDList(
        PIDLIST_ABSOLUTE *ppidl) = 0;

};
#endif  /* __IPersistIDList_INTERFACE_DEFINED__ */

#ifndef __IEnumShellItems_INTERFACE_DEFINED__
#define __IEnumShellItems_INTERFACE_DEFINED__
interface IEnumShellItems : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE Next(
        ULONG celt,
        IShellItem **rgelt,
        ULONG *pceltFetched) = 0;

    virtual HRESULT STDMETHODCALLTYPE Skip(
        ULONG celt) = 0;

    virtual HRESULT STDMETHODCALLTYPE Reset(
        ) = 0;

    virtual HRESULT STDMETHODCALLTYPE Clone(
        IEnumShellItems **ppenum) = 0;

};
#endif

#ifndef __IShellItemArray_INTERFACE_DEFINED__
#define __IShellItemArray_INTERFACE_DEFINED__
enum {
    SIATTRIBFLAGS_AND = 0x1,
    SIATTRIBFLAGS_OR = 0x2,
    SIATTRIBFLAGS_APPCOMPAT = 0x3,
    SIATTRIBFLAGS_MASK = 0x3,
    SIATTRIBFLAGS_ALLITEMS = 0x4000
};
typedef DWORD SIATTRIBFLAGS;

interface IShellItemArray : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE BindToHandler(
        IBindCtx *pbc,
        REFGUID bhid,
        REFIID riid,
        void **ppvOut) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetPropertyStore(
        GETPROPERTYSTOREFLAGS flags,
        REFIID riid,
        void **ppv) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetPropertyDescriptionList(
        REFPROPERTYKEY keyType,
        REFIID riid,
        void **ppv) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetAttributes(
        SIATTRIBFLAGS AttribFlags,
        SFGAOF sfgaoMask,
        SFGAOF *psfgaoAttribs) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetCount(
        DWORD *pdwNumItems) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetItemAt(
        DWORD dwIndex,
        IShellItem **ppsi) = 0;

    virtual HRESULT STDMETHODCALLTYPE EnumItems(
        IEnumShellItems **ppenumShellItems) = 0;
};
#endif

#ifndef __IModalWindow_INTERFACE_DEFINED__
#define __IModalWindow_INTERFACE_DEFINED__
interface IModalWindow : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE Show(
        HWND hwndOwner) = 0;

};
#endif

#ifndef __IFileDialog_FWD_DEFINED__
#define __IFileDialog_FWD_DEFINED__
typedef interface IFileDialog IFileDialog;
#ifdef __cplusplus
interface IFileDialog;
#endif /* __cplusplus */
#endif

#ifndef FD_DEFINED
#define FD_DEFINED
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
#endif

#ifndef __IFileDialogEvents_INTERFACE_DEFINED__
#define __IFileDialogEvents_INTERFACE_DEFINED__
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
#endif

typedef struct _COMDLG_FILTERSPEC {
    LPCWSTR pszName;
    LPCWSTR pszSpec;
} COMDLG_FILTERSPEC;

interface IShellItemFilter;
interface IPropertyStore;
interface IPropertyDescriptionList;
interface IFileOperationProgressSink;

#ifndef __IFileDialog_INTERFACE_DEFINED__
#define __IFileDialog_INTERFACE_DEFINED__
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
#endif

#ifndef __IFileOpenDialog_INTERFACE_DEFINED__
#define __IFileOpenDialog_INTERFACE_DEFINED__
interface IFileOpenDialog : public IFileDialog
{
    virtual HRESULT STDMETHODCALLTYPE GetResults(
        IShellItemArray **ppenum) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetSelectedItems(
        IShellItemArray **ppsai) = 0;

};
#endif

#ifndef __IFileSaveDialog_INTERFACE_DEFINED__
#define __IFileSaveDialog_INTERFACE_DEFINED__
interface IFileSaveDialog : public IFileDialog
{
    virtual HRESULT STDMETHODCALLTYPE SetSaveAsItem(
        IShellItem *psi) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetProperties(
        IPropertyStore *pStore) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetCollectedProperties(
        IPropertyDescriptionList *pList,
        WINBOOL fAppendDefault) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetProperties(
        IPropertyStore **ppStore) = 0;

    virtual HRESULT STDMETHODCALLTYPE ApplyProperties(
        IShellItem *psi,
        IPropertyStore *pStore,
        HWND hwnd,
        IFileOperationProgressSink *pSink) = 0;
};
#endif

#endif  // ndef SHOBJIDL_H_
