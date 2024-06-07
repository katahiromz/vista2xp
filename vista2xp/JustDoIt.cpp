// JustDoIt.cpp
// This file is public domain software.
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>.
#include "targetverxp.h"
#include "ExeImage.hpp"
#include <shlwapi.h>
#include <strsafe.h>
#include "resource.h"

INT GetDllCount(VOID);
LPTSTR GetDllSource(INT i);
LPCTSTR GetNewDllNames(INT i);

typedef bool (*FN_do_dll)(codereverse::ExeImage& image, size_t i, char *name);

bool do_kernel32(codereverse::ExeImage& image, size_t i, char *name)
{
    std::vector<codereverse::ImportSymbol> symbols;
    if (!image.get_import_symbols(i, symbols))
    {
        return false;
    }

    for (size_t k = 0; k < symbols.size(); ++k)
    {
        codereverse::ImportSymbol& symbol = symbols[k];
        if (symbol.Name.wImportByName)
        {
            if (memcmp(symbol.pszName, "Reg", 3) == 0 ||
                memcmp(symbol.pszName, "K32", 3) == 0 ||
                memcmp(symbol.pszName, "time", 4) == 0 ||
                lstrcmpA(symbol.pszName, "CreateProcessAsUserA") == 0 ||
                lstrcmpA(symbol.pszName, "CreateProcessAsUserW") == 0 ||
                lstrcmpA(symbol.pszName, "GetVersion") == 0 ||
                lstrcmpA(symbol.pszName, "GetVersionExA") == 0 ||
                lstrcmpA(symbol.pszName, "GetVersionExW") == 0 ||
                lstrcmpA(symbol.pszName, "OpenProcessToken") == 0 ||
                lstrcmpA(symbol.pszName, "OpenThreadToken") == 0 ||
                lstrcmpA(symbol.pszName, "SetThreadToken") == 0 ||
                lstrcmpA(symbol.pszName, "GetTickCount64") == 0 ||
                lstrcmpA(symbol.pszName, "QueryFullProcessImageNameA") == 0 ||
                lstrcmpA(symbol.pszName, "QueryFullProcessImageNameW") == 0 ||
                lstrcmpA(symbol.pszName, "IsWow64Process") == 0 ||
                lstrcmpA(symbol.pszName, "InitializeSRWLock") == 0 ||
                lstrcmpA(symbol.pszName, "AcquireSRWLockExclusive") == 0 ||
                lstrcmpA(symbol.pszName, "AcquireSRWLockShared") == 0 ||
                lstrcmpA(symbol.pszName, "TryAcquireSRWLockExclusive") == 0 ||
                lstrcmpA(symbol.pszName, "TryAcquireSRWLockShared") == 0 ||
                lstrcmpA(symbol.pszName, "ReleaseSRWLockExclusive") == 0 ||
                lstrcmpA(symbol.pszName, "ReleaseSRWLockShared") == 0 ||
                lstrcmpA(symbol.pszName, "InitOnceInitialize") == 0 ||
                lstrcmpA(symbol.pszName, "InitOnceExecuteOnce") == 0 ||
                lstrcmpA(symbol.pszName, "InitializeConditionVariable") == 0 ||
                lstrcmpA(symbol.pszName, "SleepConditionVariableCS") == 0 ||
                lstrcmpA(symbol.pszName, "SleepConditionVariableSRW") == 0 ||
                lstrcmpA(symbol.pszName, "WakeConditionVariable") == 0 ||
                lstrcmpA(symbol.pszName, "WakeAllConditionVariable") == 0 ||
                lstrcmpA(symbol.pszName, "GetThreadUILanguage") == 0 ||
                lstrcmpA(symbol.pszName, "SetThreadUILanguage") == 0 ||
                lstrcmpA(symbol.pszName, "CompareStringEx") == 0 ||
                lstrcmpA(symbol.pszName, "CreateEventExA") == 0 ||
                lstrcmpA(symbol.pszName, "CreateEventExW") == 0 ||
                lstrcmpA(symbol.pszName, "SetFileInformationByHandle") == 0 ||
                lstrcmpA(symbol.pszName, "GetFileInformationByHandleEx") == 0 ||
                lstrcmpA(symbol.pszName, "OpenFileById") == 0 ||
                lstrcmpA(symbol.pszName, "GetFinalPathNameByHandleA") == 0 ||
                lstrcmpA(symbol.pszName, "GetFinalPathNameByHandleW") == 0 ||
                lstrcmpA(symbol.pszName, "InitializeCriticalSectionEx") == 0)
            {
                if (lstrcmpiA(name, "kernel32") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xker32"), "v2xker32");
                else if (lstrcmpiA(name, "kernel32.dll") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xker32.dll"), "v2xker32.dll");
                return true;
            }
        }
    }

    return false;
}

bool do_comctl32(codereverse::ExeImage& image, size_t i, char *name)
{
    std::vector<codereverse::ImportSymbol> symbols;
    if (!image.get_import_symbols(i, symbols))
    {
        return false;
    }

    for (size_t k = 0; k < symbols.size(); ++k)
    {
        codereverse::ImportSymbol& symbol = symbols[k];
        if (symbol.Name.wImportByName)
        {
            if (lstrcmpA(symbol.pszName, "TaskDialog") == 0 ||
                lstrcmpA(symbol.pszName, "TaskDialogIndirect") == 0)
            {
                if (lstrcmpiA(name, "comctl32") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xctl32"), "v2xctl32");
                else if (lstrcmpiA(name, "comctl32.dll") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xctl32.dll"), "v2xctl32.dll");
                return true;
            }
        }
    }

    return false;
}

bool do_user32(codereverse::ExeImage& image, size_t i, char *name)
{
    std::vector<codereverse::ImportSymbol> symbols;
    if (!image.get_import_symbols(i, symbols))
    {
        return false;
    }

    for (size_t k = 0; k < symbols.size(); ++k)
    {
        codereverse::ImportSymbol& symbol = symbols[k];
        if (symbol.Name.wImportByName)
        {
            if (lstrcmpA(symbol.pszName, "ChangeWindowMessageFilter") == 0 ||
                lstrcmpA(symbol.pszName, "ChangeWindowMessageFilterEx") == 0 ||
                lstrcmpA(symbol.pszName, "GetDpiForWindow") == 0 ||
                lstrcmpA(symbol.pszName, "SetThreadDpiAwarenessContext") == 0)
            {
                if (lstrcmpiA(name, "user32") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xu32"), "v2xu32");
                else if (lstrcmpiA(name, "user32.dll") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xu32.dll"), "v2xu32.dll");
                return true;
            }
        }
    }

    return false;
}

bool do_ole32(codereverse::ExeImage& image, size_t i, char *name)
{
    std::vector<codereverse::ImportSymbol> symbols;
    if (!image.get_import_symbols(i, symbols))
    {
        return false;
    }

    for (size_t k = 0; k < symbols.size(); ++k)
    {
        codereverse::ImportSymbol& symbol = symbols[k];
        if (symbol.Name.wImportByName)
        {
            if (lstrcmpA(symbol.pszName, "CoCreateInstance") == 0)
            {
                if (lstrcmpiA(name, "ole32") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xol"), "v2xol");
                else if (lstrcmpiA(name, "ole32.dll") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xol.dll"), "v2xol.dll");
                return true;
            }
        }
    }

    return false;
}

bool do_shell32(codereverse::ExeImage& image, size_t i, char *name)
{
    std::vector<codereverse::ImportSymbol> symbols;
    if (!image.get_import_symbols(i, symbols))
    {
        return false;
    }

    for (size_t k = 0; k < symbols.size(); ++k)
    {
        codereverse::ImportSymbol& symbol = symbols[k];
        if (symbol.Name.wImportByName)
        {
            if (lstrcmpA(symbol.pszName, "SHCreateShellItemArray") == 0 ||
                lstrcmpA(symbol.pszName, "SHCreateShellItemArrayFromDataObject") == 0 ||
                lstrcmpA(symbol.pszName, "SHCreateShellItemArrayFromIDLists") == 0 ||
                lstrcmpA(symbol.pszName, "SHCreateShellItemArrayFromShellItem") == 0 ||
                lstrcmpA(symbol.pszName, "SHCreateItemFromParsingName") == 0 ||
                lstrcmpA(symbol.pszName, "SHCreateItemWithParent") == 0 ||
                lstrcmpA(symbol.pszName, "SHCreateItemFromIDList") == 0 ||
                lstrcmpA(symbol.pszName, "SHCreateShellItem") == 0 ||
                lstrcmpA(symbol.pszName, "SHGetNameFromIDList") == 0)
            {
                if (lstrcmpiA(name, "shell32") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xsh32"), "v2xsh32");
                else if (lstrcmpiA(name, "shell32.dll") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xsh32.dll"), "v2xsh32.dll");
                return true;
            }
        }
    }

    return false;
}

bool do_msvcrt(codereverse::ExeImage& image, size_t i, char *name)
{
    std::vector<codereverse::ImportSymbol> symbols;
    if (!image.get_import_symbols(i, symbols))
    {
        return false;
    }

    for (size_t k = 0; k < symbols.size(); ++k)
    {
        codereverse::ImportSymbol& symbol = symbols[k];
        if (symbol.Name.wImportByName)
        {
            if (lstrcmpA(symbol.pszName, "wcsnlen") == 0 ||
                lstrcmpA(symbol.pszName, "memmove_s") == 0 ||
                lstrcmpA(symbol.pszName, "memcpy_s") == 0 ||
                lstrcmpA(symbol.pszName, "__CxxFrameHandler3") == 0 ||
                lstrcmpA(symbol.pszName, "_except_handler4_common") == 0)
            {
                if (lstrcmpiA(name, "msvcrt") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xcrt"), "v2xcrt");
                else if (lstrcmpiA(name, "msvcrt.dll") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xcrt.dll"), "v2xcrt.dll");
                return true;
            }
        }
    }

    return false;
}

bool do_advapi32(codereverse::ExeImage& image, size_t i, char *name)
{
    std::vector<codereverse::ImportSymbol> symbols;
    if (!image.get_import_symbols(i, symbols))
    {
        return false;
    }

    for (size_t k = 0; k < symbols.size(); ++k)
    {
        codereverse::ImportSymbol& symbol = symbols[k];
        if (symbol.Name.wImportByName)
        {
            if (lstrcmpA(symbol.pszName, "RegCopyTreeW") == 0 ||
                lstrcmpA(symbol.pszName, "RegDeleteKeyExA") == 0 ||
                lstrcmpA(symbol.pszName, "RegDeleteKeyExW") == 0 ||
                lstrcmpA(symbol.pszName, "RegDeleteTreeA") == 0 ||
                lstrcmpA(symbol.pszName, "RegDeleteTreeW") == 0 ||
                lstrcmpA(symbol.pszName, "RegSetKeyValueW") == 0 ||
                lstrcmpA(symbol.pszName, "RegLoadMUIStringW") == 0 ||
                lstrcmpA(symbol.pszName, "RegLoadMUIStringA") == 0)
            {
                if (lstrcmpiA(name, "advapi32") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xadv32"), "v2xadv32");
                else if (lstrcmpiA(name, "advapi32.dll") == 0)
                    StringCbCopyA(const_cast<char *>(name), sizeof("v2xadv32.dll"), "v2xadv32.dll");
                return true;
            }
        }
    }

    return false;
}

HRESULT JustDoIt(HWND hwnd, LPCTSTR pszFile)
{
    LPCTSTR pszTitle = PathFindFileName(pszFile);

    size_t cDLLs = GetDllCount();
    for (size_t i = 0; i < cDLLs; ++i)
    {
        if (lstrcmpi(pszTitle, GetNewDllNames(i)) == 0)
        {
            return S_FALSE;
        }
    }

    TCHAR szText[128];
    codereverse::ExeImage image;
    if (!image.load(pszFile))
    {
        LoadString(NULL, IDS_CANTLOAD, szText, _countof(szText));
        MessageBox(hwnd, szText, NULL, MB_ICONERROR);
        return E_FAIL;
    }

    if (image.is_64bit())
    {
        LoadString(NULL, IDS_NOTSUP64BIT, szText, _countof(szText));
        MessageBox(hwnd, szText, TEXT("Warning"), MB_ICONWARNING);
        return S_FALSE;
    }

    std::vector<const char *> names;
    if (!image.get_import_dll_names(names))
    {
        return S_OK;
    }

    LPCSTR DllNames1[] =
    {
        "kernel32",
        "comctl32",
        "user32",
        "ole32",
        "shell32",
        "msvcrt",
        "advapi32",
    };
    LPCSTR DllNames2[] =
    {
        "kernel32.dll",
        "comctl32.dll",
        "user32.dll",
        "ole32.dll",
        "shell32.dll",
        "msvcrt.dll",
        "advapi32.dll",
    };
    FN_do_dll do_dll_funcs[] =
    {
        do_kernel32,
        do_comctl32,
        do_user32,
        do_ole32,
        do_shell32,
        do_msvcrt,
        do_advapi32,
    };
    bool found[7] = { false };

    assert(cDLLs == _countof(found));
    assert(cDLLs == _countof(DllNames1));
    assert(cDLLs == _countof(DllNames2));
    assert(cDLLs == _countof(do_dll_funcs));

    for (UINT k = 0; k < cDLLs; ++k)
    {
        for (DWORD i = 0; i < names.size(); ++i)
        {
            if (lstrcmpiA(names[i], DllNames1[k]) == 0 ||
                lstrcmpiA(names[i], DllNames2[k]) == 0)
            {
                found[k] = do_dll_funcs[k](image, i, const_cast<char *>(names[i]));
                break;
            }
        }
    }

    TCHAR szPath[MAX_PATH];
    StringCbCopy(szPath, sizeof(szPath), pszFile);
    LPTSTR pch = PathFindFileName(szPath);
    if (pch == NULL)
        return E_FAIL;

    for (UINT k = 0; k < cDLLs; ++k)
    {
        if (!found[k])
            continue;

        // cut off file title
        *pch = 0;

        // create backup
        PathAppend(szPath, TEXT("Vista2XP-Backup"));
        CreateDirectory(szPath, NULL);
        PathAppend(szPath, pszTitle);
        CopyFile(pszFile, szPath, TRUE);

        // cut off file title
        *pch = 0;

        PathAppend(szPath, GetNewDllNames(k));
        if (!PathFileExists(szPath) && !CopyFile(GetDllSource(k), szPath, FALSE))
            return E_FAIL;
    }

    bool version_fix = false;
    {
        DWORD ver, preferred = MAKELONG(1, 5);
        if (IMAGE_OPTIONAL_HEADER64 *optional64 = image.get_optional64())
        {
            ver = MAKELONG(optional64->MinorOperatingSystemVersion, optional64->MajorOperatingSystemVersion);
            if (ver > preferred)
            {
                optional64->MajorOperatingSystemVersion = 5;
                optional64->MinorOperatingSystemVersion = 1;
                version_fix = true;
            }
            ver = MAKELONG(optional64->MinorSubsystemVersion, optional64->MajorSubsystemVersion);
            if (ver > preferred)
            {
                optional64->MajorSubsystemVersion = 5;
                optional64->MinorSubsystemVersion = 1;
                version_fix = true;
            }
        }
        else if (IMAGE_OPTIONAL_HEADER32 *optional32 = image.get_optional32())
        {
            ver = MAKELONG(optional32->MinorOperatingSystemVersion, optional32->MajorOperatingSystemVersion);
            if (ver > preferred)
            {
                optional32->MajorOperatingSystemVersion = 5;
                optional32->MinorOperatingSystemVersion = 1;
                version_fix = true;
            }
            ver = MAKELONG(optional32->MinorSubsystemVersion, optional32->MajorSubsystemVersion);
            if (ver > preferred)
            {
                optional32->MajorSubsystemVersion = 5;
                optional32->MinorSubsystemVersion = 1;
                version_fix = true;
            }
        }
    }

    if (found && version_fix)
    {
        if (!image.do_reverse_map() || !image.save(pszFile))
        {
            LoadString(NULL, IDS_CANTWRITE, szText, _countof(szText));
            MessageBox(hwnd, szText, NULL, MB_ICONERROR);
            return E_FAIL;
        }
    }

    return S_OK;
}
