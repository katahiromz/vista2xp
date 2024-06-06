/* v2xadv32.c --- v2xadv32.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2024 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetverxp.h"
#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <strsafe.h>

#ifndef ARRAYSIZE
    #define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

static HINSTANCE s_hinstDLL;
static HINSTANCE s_hADVAPI32;

// RegCopyTreeW
typedef LONG (WINAPI *FN_RegCopyTreeW)(HKEY, LPCWSTR, HKEY);
FN_RegCopyTreeW s_pRegCopyTreeW = NULL;

// RegDeleteKeyExA
typedef LONG (WINAPI *FN_RegDeleteKeyExA)(HKEY, LPCSTR, REGSAM, DWORD);
FN_RegDeleteKeyExA s_pRegDeleteKeyExA = NULL;

// RegDeleteKeyExW
typedef LONG (WINAPI *FN_RegDeleteKeyExW)(HKEY, LPCWSTR, REGSAM, DWORD);
FN_RegDeleteKeyExW s_pRegDeleteKeyExW = NULL;
 
// RegDeleteTreeA
typedef LONG (WINAPI *FN_RegDeleteTreeA)(HKEY, LPCSTR);
FN_RegDeleteTreeA s_pRegDeleteTreeA = NULL;

// RegDeleteTreeW
typedef LONG (WINAPI *FN_RegDeleteTreeW)(HKEY, LPCWSTR);
FN_RegDeleteTreeW s_pRegDeleteTreeW = NULL;

// RegSetKeyValueW
typedef LONG (WINAPI *FN_RegSetKeyValueW)(HKEY, LPCWSTR, LPCWSTR, DWORD, LPCVOID, DWORD);
FN_RegSetKeyValueW s_pRegSetKeyValueW = NULL;

// RegLoadMUIStringW
typedef LONG (WINAPI *FN_RegLoadMUIStringW)(HKEY, LPCWSTR, LPWSTR, DWORD, LPDWORD, DWORD, LPCWSTR pszDirectory);
FN_RegLoadMUIStringW s_pRegLoadMUIStringW = NULL;

// RegLoadMUIStringA
typedef LONG (WINAPI *FN_RegLoadMUIStringA)(HKEY, LPCSTR, LPSTR, DWORD, LPDWORD, DWORD, LPCSTR pszDirectory);
FN_RegLoadMUIStringA s_pRegLoadMUIStringA = NULL;

LONG WINAPI
RegCopyTreeWForXP(IN HKEY hKeySrc,
                  IN LPCWSTR lpSubKey  OPTIONAL,
                  IN HKEY hKeyDest)
{
    if (s_pRegCopyTreeW && DO_FALLBACK)
        return s_pRegCopyTreeW(hKeySrc, lpSubKey, hKeyDest);

    return ERROR_CALL_NOT_IMPLEMENTED;
}

static LONG DoRegDeleteTreeA(HKEY hKey, LPCSTR lpSubKey)
{
    LONG ret;
    DWORD cchSubKeyMax, cchValueMax;
    DWORD cchMax, cch;
    CHAR szNameBuf[MAX_PATH], *pszName = szNameBuf;
    HKEY hSubKey = hKey;

    if (lpSubKey != NULL)
    {
        ret = RegOpenKeyExA(hKey, lpSubKey, 0, KEY_READ, &hSubKey);
        if (ret)
            return ret;
    }

    ret = RegQueryInfoKeyA(hSubKey, NULL, NULL, NULL, NULL,
                           &cchSubKeyMax, NULL, NULL, &cchValueMax, NULL, NULL, NULL);
    if (ret)
        goto cleanup;

    cchSubKeyMax++;
    cchValueMax++;
    cchMax = max(cchSubKeyMax, cchValueMax);
    if (cchMax > sizeof(szNameBuf) / sizeof(CHAR))
    {
        pszName = (LPSTR)HeapAlloc(GetProcessHeap(), 0, cchMax *  sizeof(CHAR));
        if (pszName == NULL)
        {
            ret = ERROR_NOT_ENOUGH_MEMORY;
            goto cleanup;
        }
    }

    while (TRUE)
    {
        cch = cchMax;
        if (RegEnumKeyExA(hSubKey, 0, pszName, &cch, NULL, NULL, NULL, NULL))
            break;

        ret = DoRegDeleteTreeA(hSubKey, pszName);
        if (ret)
            goto cleanup;
    }

    if (lpSubKey != NULL)
        ret = RegDeleteKeyA(hKey, lpSubKey);
    else
        while(TRUE)
        {
            cch = cchMax;
            if (RegEnumValueA(hKey, 0, pszName, &cch, NULL, NULL, NULL, NULL))
                break;

            ret = RegDeleteValueA(hKey, pszName);
            if (ret)
                goto cleanup;
        }

cleanup:
    if (pszName != szNameBuf)
        HeapFree(GetProcessHeap(), 0, pszName);
    if (lpSubKey != NULL)
        RegCloseKey(hSubKey);
    return ret;
}

static LONG DoRegDeleteTreeW(HKEY hKey, LPCWSTR lpSubKey)
{
    LONG ret;
    DWORD cchSubKeyMax, cchValueMax;
    DWORD cchMax, cch;
    WCHAR szNameBuf[MAX_PATH], *pszName = szNameBuf;
    HKEY hSubKey = hKey;

    if (lpSubKey != NULL)
    {
        ret = RegOpenKeyExW(hKey, lpSubKey, 0, KEY_READ, &hSubKey);
        if (ret)
            return ret;
    }

    ret = RegQueryInfoKeyW(hSubKey, NULL, NULL, NULL, NULL,
                           &cchSubKeyMax, NULL, NULL, &cchValueMax, NULL, NULL, NULL);
    if (ret)
        goto cleanup;

    cchSubKeyMax++;
    cchValueMax++;
    cchMax = max(cchSubKeyMax, cchValueMax);
    if (cchMax > sizeof(szNameBuf) / sizeof(WCHAR))
    {
        pszName = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, cchMax *  sizeof(WCHAR));
        if (pszName == NULL)
        {
            ret = ERROR_NOT_ENOUGH_MEMORY;
            goto cleanup;
        }
    }

    while (TRUE)
    {
        cch = cchMax;
        if (RegEnumKeyExW(hSubKey, 0, pszName, &cch, NULL, NULL, NULL, NULL))
            break;

        ret = DoRegDeleteTreeW(hSubKey, pszName);
        if (ret)
            goto cleanup;
    }

    if (lpSubKey != NULL)
        ret = RegDeleteKeyW(hKey, lpSubKey);
    else
        while(TRUE)
        {
            cch = cchMax;
            if (RegEnumValueW(hKey, 0, pszName, &cch, NULL, NULL, NULL, NULL))
                break;

            ret = RegDeleteValueW(hKey, pszName);
            if (ret)
                goto cleanup;
        }

cleanup:
    if (pszName != szNameBuf)
        HeapFree(GetProcessHeap(), 0, pszName);
    if (lpSubKey != NULL)
        RegCloseKey(hSubKey);
    return ret;
}

LONG WINAPI
RegDeleteKeyExAForXP(
    HKEY    hKey,
    LPCSTR  lpSubKey,
    REGSAM  samDesired,
    DWORD   Reserved)
{
    if (s_pRegDeleteKeyExA && DO_FALLBACK)
        return s_pRegDeleteKeyExA(hKey, lpSubKey, samDesired, Reserved);

    if (!lpSubKey)
        return ERROR_INVALID_PARAMETER;

    return DoRegDeleteTreeA(hKey, lpSubKey);
}

LONG WINAPI
RegDeleteKeyExWForXP(
    HKEY    hKey,
    LPCWSTR lpSubKey,
    REGSAM  samDesired,
    DWORD   Reserved)
{
    if (s_pRegDeleteKeyExW && DO_FALLBACK)
        return s_pRegDeleteKeyExW(hKey, lpSubKey, samDesired, Reserved);

    if (!lpSubKey)
        return ERROR_INVALID_PARAMETER;

    return DoRegDeleteTreeW(hKey, lpSubKey);
}


LONG WINAPI
RegDeleteTreeAForXP(IN HKEY hKey,
                    IN LPCSTR lpSubKey  OPTIONAL)
{
    if (s_pRegDeleteTreeA && DO_FALLBACK)
        return s_pRegDeleteTreeA(hKey, lpSubKey);

    return DoRegDeleteTreeA(hKey, lpSubKey);
}

LONG WINAPI
RegDeleteTreeWForXP(IN HKEY hKey,
                    IN LPCWSTR lpSubKey  OPTIONAL)
{
    if (s_pRegDeleteTreeW && DO_FALLBACK)
        return s_pRegDeleteTreeW(hKey, lpSubKey);

    return DoRegDeleteTreeW(hKey, lpSubKey);
}

LONG WINAPI
RegSetKeyValueWForXP(IN HKEY hKey,
                     IN LPCWSTR lpSubKey  OPTIONAL,
                     IN LPCWSTR lpValueName  OPTIONAL,
                     IN DWORD dwType,
                     IN LPCVOID lpData  OPTIONAL,
                     IN DWORD cbData)
{
    HKEY hSubKey;
    LONG error;

    if (s_pRegSetKeyValueW && DO_FALLBACK)
        return s_pRegSetKeyValueW(hKey, lpSubKey, lpValueName, dwType, lpData, cbData);

    error = RegOpenKeyExW(hKey, lpSubKey, 0, KEY_WRITE, &hSubKey);
    if (error)
        return error;

    error = RegSetValueExW(hSubKey, lpValueName, 0, dwType, (BYTE*)lpData, cbData);
    RegCloseKey(hKey);

    return error;
}

LONG WINAPI
RegLoadMUIStringWForXP(
    IN HKEY hKey,
    IN LPCWSTR pszValue  OPTIONAL,
    OUT LPWSTR pszOutBuf,
    IN DWORD cbOutBuf,
    OUT LPDWORD pcbData OPTIONAL,
    IN DWORD Flags,
    IN LPCWSTR pszDirectory  OPTIONAL)
{
    DWORD dwValueType, cbData;
    LPWSTR pwszTempBuffer = NULL, pwszExpandedBuffer = NULL;
    LONG result;

    if (s_pRegLoadMUIStringW && DO_FALLBACK)
        return s_pRegLoadMUIStringW(hKey, pszValue, pszOutBuf, cbOutBuf, pcbData, Flags, pszDirectory);

    /* Parameter sanity checks. */
    if (!hKey || !pszOutBuf)
        return ERROR_INVALID_PARAMETER;

    if (pszDirectory && *pszDirectory)
        return ERROR_INVALID_PARAMETER;

    /* Check for value existence and correctness of it's type, allocate a buffer and load it. */
    result = RegQueryValueExW(hKey, pszValue, NULL, &dwValueType, NULL, &cbData);
    if (result != ERROR_SUCCESS) goto cleanup;
    if (!(dwValueType == REG_SZ || dwValueType == REG_EXPAND_SZ) || !cbData)
    {
        result = ERROR_FILE_NOT_FOUND;
        goto cleanup;
    }
    pwszTempBuffer = HeapAlloc(GetProcessHeap(), 0, cbData);
    if (!pwszTempBuffer)
    {
        result = ERROR_NOT_ENOUGH_MEMORY;
        goto cleanup;
    }
    result = RegQueryValueExW(hKey, pszValue, NULL, &dwValueType, (LPBYTE)pwszTempBuffer, &cbData);
    if (result != ERROR_SUCCESS) goto cleanup;

    /* Expand environment variables, if appropriate, or copy the original string over. */
    if (dwValueType == REG_EXPAND_SZ)
    {
        cbData = ExpandEnvironmentStringsW(pwszTempBuffer, NULL, 0) * sizeof(WCHAR);
        if (!cbData) goto cleanup;
        pwszExpandedBuffer = HeapAlloc(GetProcessHeap(), 0, cbData);
        if (!pwszExpandedBuffer)
        {
            result = ERROR_NOT_ENOUGH_MEMORY;
            goto cleanup;
        }
        ExpandEnvironmentStringsW(pwszTempBuffer, pwszExpandedBuffer, cbData);
    }
    else
    {
        pwszExpandedBuffer = HeapAlloc(GetProcessHeap(), 0, cbData);
        memcpy(pwszExpandedBuffer, pwszTempBuffer, cbData);
    }

    /* If the value references a resource based string, parse the value and load the string.
     * Else just copy over the original value. */
    result = ERROR_SUCCESS;
    if (*pwszExpandedBuffer != L'@') /* '@' is the prefix for resource based string entries. */
    {
        lstrcpynW(pszOutBuf, pwszExpandedBuffer, cbOutBuf / sizeof(WCHAR));
    }
    else
    {
        WCHAR* pComma = wcsrchr(pwszExpandedBuffer, L',');
        UINT uiStringId;
        HMODULE hModule;

        /* Format of the expanded value is 'path_to_dll,-resId' */
        if (!pComma || pComma[1] != L'-')
        {
            result = ERROR_BADKEY;
            goto cleanup;
        }

        uiStringId = _wtoi(pComma + 2);
        *pComma = L'\0';

        hModule = LoadLibraryExW(pwszExpandedBuffer + 1, NULL, LOAD_LIBRARY_AS_DATAFILE);
        if (!hModule || !LoadStringW(hModule, uiStringId, pszOutBuf, cbOutBuf / sizeof(WCHAR)))
            result = ERROR_BADKEY;
        FreeLibrary(hModule);
    }

cleanup:
    HeapFree(GetProcessHeap(), 0, pwszTempBuffer);
    HeapFree(GetProcessHeap(), 0, pwszExpandedBuffer);
    return result;
}

LONG WINAPI
RegLoadMUIStringAForXP(
    IN HKEY hKey,
    IN LPCSTR pszValue  OPTIONAL,
    OUT LPSTR pszOutBuf,
    IN DWORD cbOutBuf,
    OUT LPDWORD pcbData OPTIONAL,
    IN DWORD Flags,
    IN LPCSTR pszDirectory  OPTIONAL)
{
    WCHAR valueW[MAX_PATH], baseDirW[MAX_PATH];
    WCHAR* pwszBuffer;
    DWORD cbData = cbOutBuf * sizeof(WCHAR);
    LONG result;
    LPWSTR pszValueW = NULL, pszBaseDirW = NULL;

    if (s_pRegLoadMUIStringA && DO_FALLBACK)
        return s_pRegLoadMUIStringA(hKey, pszValue, pszOutBuf, cbOutBuf, pcbData, Flags, pszDirectory);

    if (!(pwszBuffer = HeapAlloc(GetProcessHeap(), 0, cbData)))
    {
        result = ERROR_NOT_ENOUGH_MEMORY;
        goto cleanup;
    }

    if (pszValue)
    {
        MultiByteToWideChar(CP_ACP, 0, pszValue, -1, valueW, ARRAYSIZE(valueW));
        pszValueW = valueW;
    }

    if (pszDirectory)
    {
        MultiByteToWideChar(CP_ACP, 0, pszDirectory, -1, baseDirW, ARRAYSIZE(baseDirW));
        pszBaseDirW = baseDirW;
    }

    result = RegLoadMUIStringWForXP(hKey, pszValueW, pwszBuffer, cbData, NULL, Flags, pszBaseDirW);

    if (result == ERROR_SUCCESS)
    {
        cbData = WideCharToMultiByte(CP_ACP, 0, pwszBuffer, -1, pszOutBuf, cbOutBuf, NULL, NULL);
        if (pcbData)
            *pcbData = cbData;
    }

cleanup:
    HeapFree(GetProcessHeap(), 0, pwszBuffer);
    return result;
}

#define GETPROC(fn) s_p##fn = (FN_##fn)GetProcAddress(s_hADVAPI32, #fn)

BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        s_hinstDLL = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
        s_hADVAPI32 = LoadLibraryA("advapi32");
        GETPROC(RegCopyTreeW);
        GETPROC(RegDeleteKeyExA);
        GETPROC(RegDeleteKeyExW);
        GETPROC(RegDeleteTreeA);
        GETPROC(RegDeleteTreeW);
        GETPROC(RegSetKeyValueW);
        GETPROC(RegLoadMUIStringW);
        GETPROC(RegLoadMUIStringA);
        break;
    case DLL_PROCESS_DETACH:
        FreeLibrary(s_hADVAPI32);
        break;
    }
    return TRUE;
}
