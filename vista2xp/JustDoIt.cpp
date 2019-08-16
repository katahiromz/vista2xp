// JustDoIt.cpp
// This file is public domain software.
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>.
#include "targetverxp.h"
#include "ExeImage.hpp"
#include <shlwapi.h>
#include <strsafe.h>
#include "resource.h"

#ifndef ARRAYSIZE
    #define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

LPTSTR GetDllSource(INT i);

LPCTSTR GetDllNames(INT i);

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
                lstrcmpA(symbol.pszName, "OpenProcessToken") == 0 ||
                lstrcmpA(symbol.pszName, "OpenThreadToken") == 0 ||
                lstrcmpA(symbol.pszName, "SetThreadToken") == 0 ||
                lstrcmpA(symbol.pszName, "GetTickCount64") == 0 ||
                lstrcmpA(symbol.pszName, "QueryFullProcessImageNameA") == 0 ||
                lstrcmpA(symbol.pszName, "QueryFullProcessImageNameW") == 0 ||
                lstrcmpA(symbol.pszName, "IsWow64Process") == 0)
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

HRESULT JustDoIt(HWND hwnd, LPCTSTR pszFile)
{
    LPCTSTR pszTitle = PathFindFileName(pszFile);

    for (size_t i = 0; i < 6; ++i)
    {
        if (lstrcmpi(pszTitle, GetDllNames(i)) == 0)
        {
            return S_FALSE;
        }
    }

    TCHAR szText[128];
    codereverse::ExeImage image;
    if (!image.load(pszFile))
    {
        LoadString(NULL, IDS_CANTLOAD, szText, ARRAYSIZE(szText));
        MessageBox(hwnd, szText, NULL, MB_ICONERROR);
        return E_FAIL;
    }

    if (image.is_64bit())
    {
        LoadString(NULL, IDS_NOTSUP64BIT, szText, ARRAYSIZE(szText));
        MessageBox(hwnd, szText, TEXT("Warning"), MB_ICONWARNING);
        return S_FALSE;
    }

    std::vector<const char *> names;
    if (!image.get_import_dll_names(names))
    {
        return S_OK;
    }

    bool v2xker32_found = false;
    bool v2xctl32_found = false;
    bool v2xu32_found = false;
    bool v2xol_found = false;
    bool v2xsh32_found = false;
    bool v2xcrt_found = false;

    for (DWORD i = 0; i < names.size(); ++i)
    {
        if (lstrcmpiA(names[i], "kernel32.dll") == 0 ||
            lstrcmpiA(names[i], "kernel32") == 0)
        {
            v2xker32_found = do_kernel32(image, i, const_cast<char *>(names[i]));
        }
        else if (lstrcmpiA(names[i], "comctl32.dll") == 0 ||
                 lstrcmpiA(names[i], "comctl32") == 0)
        {
            v2xctl32_found = do_comctl32(image, i, const_cast<char *>(names[i]));
        }
        else if (lstrcmpiA(names[i], "user32.dll") == 0 ||
                 lstrcmpiA(names[i], "user32") == 0)
        {
            v2xu32_found = do_user32(image, i, const_cast<char *>(names[i]));
        }
        else if (lstrcmpiA(names[i], "ole32.dll") == 0 ||
                 lstrcmpiA(names[i], "ole32") == 0)
        {
            v2xol_found = do_ole32(image, i, const_cast<char *>(names[i]));
        }
        else if (lstrcmpiA(names[i], "shell32.dll") == 0 ||
                 lstrcmpiA(names[i], "shell32") == 0)
        {
            v2xsh32_found = do_shell32(image, i, const_cast<char *>(names[i]));
        }
        else if (lstrcmpiA(names[i], "msvcrt.dll") == 0 ||
                 lstrcmpiA(names[i], "msvcrt") == 0)
        {
            v2xcrt_found = do_msvcrt(image, i, const_cast<char *>(names[i]));
        }
    }

    TCHAR szPath[MAX_PATH];
    StringCbCopy(szPath, sizeof(szPath), pszFile);
    LPTSTR pch = PathFindFileName(szPath);
    if (pch == NULL)
        return E_FAIL;

    if (v2xker32_found)
    {
        // cut off file title
        *pch = 0;

        // create backup
        PathAppend(szPath, TEXT("Vista2XP-Backup"));
        CreateDirectory(szPath, NULL);
        PathAppend(szPath, pszTitle);
        CopyFile(pszFile, szPath, TRUE);

        // cut off file title
        *pch = 0;

        PathAppend(szPath, GetDllNames(0));
        if (!PathFileExists(szPath) && !CopyFile(GetDllSource(0), szPath, FALSE))
            return E_FAIL;
    }

    if (v2xctl32_found)
    {
        // cut off file title
        *pch = 0;

        // create backup
        PathAppend(szPath, TEXT("Vista2XP-Backup"));
        CreateDirectory(szPath, NULL);
        PathAppend(szPath, pszTitle);
        CopyFile(pszFile, szPath, TRUE);

        // cut off file title
        *pch = 0;

        PathAppend(szPath, GetDllNames(1));
        if (!PathFileExists(szPath) && !CopyFile(GetDllSource(1), szPath, FALSE))
            return E_FAIL;
    }

    if (v2xu32_found)
    {
        // cut off file title
        *pch = 0;

        // create backup
        PathAppend(szPath, TEXT("Vista2XP-Backup"));
        CreateDirectory(szPath, NULL);
        PathAppend(szPath, pszTitle);
        CopyFile(pszFile, szPath, TRUE);

        // cut off file title
        *pch = 0;

        PathAppend(szPath, GetDllNames(2));
        if (!PathFileExists(szPath) && !CopyFile(GetDllSource(2), szPath, FALSE))
            return E_FAIL;
    }

    if (v2xol_found)
    {
        // cut off file title
        *pch = 0;

        // create backup
        PathAppend(szPath, TEXT("Vista2XP-Backup"));
        CreateDirectory(szPath, NULL);
        PathAppend(szPath, pszTitle);
        CopyFile(pszFile, szPath, TRUE);

        // cut off file title
        *pch = 0;

        PathAppend(szPath, GetDllNames(3));
        if (!PathFileExists(szPath) && !CopyFile(GetDllSource(3), szPath, FALSE))
            return E_FAIL;
    }

    if (v2xsh32_found)
    {
        // cut off file title
        *pch = 0;

        // create backup
        PathAppend(szPath, TEXT("Vista2XP-Backup"));
        CreateDirectory(szPath, NULL);
        PathAppend(szPath, pszTitle);
        CopyFile(pszFile, szPath, TRUE);

        // cut off file title
        *pch = 0;

        PathAppend(szPath, GetDllNames(4));
        if (!PathFileExists(szPath) && !CopyFile(GetDllSource(4), szPath, FALSE))
            return E_FAIL;
    }

    if (v2xcrt_found)
    {
        // cut off file title
        *pch = 0;

        // create backup
        PathAppend(szPath, TEXT("Vista2XP-Backup"));
        CreateDirectory(szPath, NULL);
        PathAppend(szPath, pszTitle);
        CopyFile(pszFile, szPath, TRUE);

        // cut off file title
        *pch = 0;

        PathAppend(szPath, GetDllNames(5));
        if (!PathFileExists(szPath) && !CopyFile(GetDllSource(5), szPath, FALSE))
            return E_FAIL;
    }

    if (v2xker32_found || v2xctl32_found || v2xu32_found || v2xol_found ||
        v2xsh32_found || v2xcrt_found)
    {
        if (!image.do_reverse_map() || !image.save(pszFile))
        {
            LoadString(NULL, IDS_CANTWRITE, szText, ARRAYSIZE(szText));
            MessageBox(hwnd, szText, NULL, MB_ICONERROR);
            return E_FAIL;
        }
    }

    return S_OK;
}
