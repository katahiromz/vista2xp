/* v2xker32.c --- v2xker32.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetver.h"
#include <windows.h>
#include <string.h>
#include <strsafe.h>
#include <psapi.h>

#ifndef ARRAYSIZE
    #define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

static HINSTANCE s_hinstDLL;
static HINSTANCE s_hKernel32;
static BOOL s_bUseQPC;
static LARGE_INTEGER s_freq;

typedef BOOL (WINAPI *FN_IsWow64Process)(HANDLE, PBOOL);
typedef ULONGLONG (WINAPI *FN_GetTickCount64)(void);
typedef BOOL (WINAPI *FN_QueryFullProcessImageNameA)(HANDLE, DWORD, LPSTR, PDWORD);
typedef BOOL (WINAPI *FN_QueryFullProcessImageNameW)(HANDLE, DWORD, LPWSTR, PDWORD);

static FN_IsWow64Process s_pIsWow64Process;
static FN_GetTickCount64 s_pGetTickCount64;
static FN_QueryFullProcessImageNameA s_pQueryFullProcessImageNameA;
static FN_QueryFullProcessImageNameW s_pQueryFullProcessImageNameW;

BOOL WINAPI
IsWow64ProcessForXP(HANDLE hProcess, PBOOL Wow64Process)
{
    if (s_pIsWow64Process && DO_FALLBACK)
        return (*s_pIsWow64Process)(hProcess, Wow64Process);
    return FALSE;
}

ULONGLONG WINAPI GetTickCount64ForXP(void)
{
    LARGE_INTEGER count;
    if (s_pGetTickCount64 && DO_FALLBACK)
    {
        return (*s_pGetTickCount64)();
    }
    else if (s_bUseQPC)
    {
        QueryPerformanceCounter(&count);
        return (count.QuadPart * 1000) / s_freq.QuadPart;
    }
    else
    {
        return GetTickCount();
    }
}

static BOOL Win32PathFromNTPathA(CHAR *pszDest, size_t cchDestMax, LPCSTR pszSrc)
{
    INT cchSrc, cchLen;
    CHAR szNTPath[MAX_PATH], szDrive[MAX_PATH] = "A:";
    CHAR chDrive;

    StringCbCopyA(pszDest, cchDestMax, pszSrc);

    cchSrc = lstrlenA(pszSrc);
    for (chDrive = 'A'; chDrive <= 'Z'; ++chDrive)
    {
        szDrive[0] = chDrive;
        szNTPath[0] = 0;
        if (QueryDosDeviceA(szDrive, szNTPath, ARRAYSIZE(szNTPath)))
        {
            cchLen = lstrlenA(szNTPath);
            if (memcmp(szNTPath, pszSrc, cchLen * sizeof(CHAR)) == 0)
            {
                StringCchCopyA(pszDest, cchDestMax, szDrive);
                StringCchCatA(pszDest, cchDestMax, &pszSrc[cchLen]);
                return TRUE;
            }
        }
    }
    return FALSE;
}

static BOOL Win32PathFromNTPathW(WCHAR *pszDest, size_t cchDestMax, LPCWSTR pszSrc)
{
    INT cchSrc, cchLen;
    WCHAR szNTPath[MAX_PATH], szDrive[MAX_PATH] = L"A:";
    WCHAR chDrive;

    StringCbCopyW(pszDest, cchDestMax, pszSrc);

    cchSrc = lstrlenW(pszSrc);
    for (chDrive = L'A'; chDrive <= L'Z'; ++chDrive)
    {
        szDrive[0] = chDrive;
        szNTPath[0] = 0;
        if (QueryDosDeviceW(szDrive, szNTPath, ARRAYSIZE(szNTPath)))
        {
            cchLen = lstrlenW(szNTPath);
            if (memcmp(szNTPath, pszSrc, cchLen * sizeof(WCHAR)) == 0)
            {
                StringCchCopyW(pszDest, cchDestMax, szDrive);
                StringCchCatW(pszDest, cchDestMax, &pszSrc[cchLen]);
                return TRUE;
            }
        }
    }
    return FALSE;
}

#ifndef PROCESS_NAME_NATIVE
    #define PROCESS_NAME_NATIVE 0x00000001
#endif

BOOL WINAPI
QueryFullProcessImageNameAForXP(
    HANDLE hProcess,
    DWORD  dwFlags,
    LPSTR  lpExeName,
    PDWORD lpdwSize)
{
    CHAR szPath[MAX_PATH];
    BOOL ret;

    if (s_pQueryFullProcessImageNameA && DO_FALLBACK)
    {
        return (*s_pQueryFullProcessImageNameA)(hProcess, dwFlags, lpExeName, lpdwSize);
    }

    if (dwFlags & PROCESS_NAME_NATIVE)
    {
        ret = GetProcessImageFileNameA(hProcess, lpExeName, *lpdwSize);
    }
    else
    {
        ret = GetProcessImageFileNameA(hProcess, szPath, ARRAYSIZE(szPath));
        Win32PathFromNTPathA(lpExeName, *lpdwSize, szPath);
    }

    if (ret)
        *lpdwSize = lstrlenA(lpExeName);
    else
        *lpdwSize = 0;

    return ret;
}

BOOL WINAPI
QueryFullProcessImageNameWForXP(
    HANDLE hProcess,
    DWORD  dwFlags,
    LPWSTR lpExeName,
    PDWORD lpdwSize)
{
    WCHAR szPath[MAX_PATH];
    BOOL ret;

    if (s_pQueryFullProcessImageNameW && DO_FALLBACK)
    {
        return (*s_pQueryFullProcessImageNameW)(hProcess, dwFlags, lpExeName, lpdwSize);
    }

    if (dwFlags & PROCESS_NAME_NATIVE)
    {
        ret = GetProcessImageFileNameW(hProcess, lpExeName, *lpdwSize);
    }
    else
    {
        ret = GetProcessImageFileNameW(hProcess, szPath, ARRAYSIZE(szPath));
        Win32PathFromNTPathW(lpExeName, *lpdwSize, szPath);
    }

    if (ret)
        *lpdwSize = lstrlenW(lpExeName);
    else
        *lpdwSize = 0;

    return ret;
}

BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        s_hinstDLL = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
        s_bUseQPC = QueryPerformanceFrequency(&s_freq);
        s_hKernel32 = GetModuleHandleA("kernel32");
        s_pIsWow64Process = (FN_IsWow64Process)GetProcAddress(s_hKernel32, "IsWow64Process");
        s_pGetTickCount64 = (FN_GetTickCount64)GetProcAddress(s_hKernel32, "GetTickCount64");
        s_pQueryFullProcessImageNameA = (FN_QueryFullProcessImageNameA)GetProcAddress(s_hKernel32, "QueryFullProcessImageNameA");
        s_pQueryFullProcessImageNameW = (FN_QueryFullProcessImageNameW)GetProcAddress(s_hKernel32, "QueryFullProcessImageNameW");
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
