/* v2xker32.c --- v2xker32.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetverxp.h"
#include <windows.h>
#include <string.h>
#include <strsafe.h>
#include <psapi.h>

typedef struct tagSRWLOCK_FOR_XP
{
    PVOID Ptr;
} SRWLOCK_FOR_XP, *PSRWLOCK_FOR_XP;

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

typedef VOID (WINAPI *FN_InitializeSRWLock)(PSRWLOCK_FOR_XP SRWLock);
typedef VOID (WINAPI *FN_AcquireSRWLockExclusive)(PSRWLOCK_FOR_XP SRWLock);
typedef VOID (WINAPI *FN_AcquireSRWLockShared)(PSRWLOCK_FOR_XP SRWLock);
typedef BOOLEAN (WINAPI *FN_TryAcquireSRWLockExclusive)(PSRWLOCK_FOR_XP SRWLock);
typedef BOOLEAN (WINAPI *FN_TryAcquireSRWLockShared)(PSRWLOCK_FOR_XP SRWLock);
typedef VOID (WINAPI *FN_ReleaseSRWLockExclusive)(PSRWLOCK_FOR_XP SRWLock);
typedef VOID (WINAPI *FN_ReleaseSRWLockShared)(PSRWLOCK_FOR_XP SRWLock);

static FN_InitializeSRWLock s_pInitializeSRWLock;
static FN_AcquireSRWLockExclusive s_pAcquireSRWLockExclusive;
static FN_AcquireSRWLockShared s_pAcquireSRWLockShared;
static FN_TryAcquireSRWLockExclusive s_pTryAcquireSRWLockExclusive;
static FN_TryAcquireSRWLockShared s_pTryAcquireSRWLockShared;
static FN_ReleaseSRWLockExclusive s_pReleaseSRWLockExclusive;
static FN_ReleaseSRWLockShared s_pReleaseSRWLockShared;

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
        if (QueryDosDeviceA(szDrive, szNTPath, _countof(szNTPath)))
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
        if (QueryDosDeviceW(szDrive, szNTPath, _countof(szNTPath)))
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
        ret = GetProcessImageFileNameA(hProcess, szPath, _countof(szPath));
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
        ret = GetProcessImageFileNameW(hProcess, szPath, _countof(szPath));
        Win32PathFromNTPathW(lpExeName, *lpdwSize, szPath);
    }

    if (ret)
        *lpdwSize = lstrlenW(lpExeName);
    else
        *lpdwSize = 0;

    return ret;
}

DWORD WINAPI
GetVersionHacked(void)
{
    // Win10
    return 0x23F00206;
}

BOOL WINAPI
GetVersionExAHacked(LPOSVERSIONINFOA osver)
{
    if (osver->dwOSVersionInfoSize != sizeof(OSVERSIONINFOA) ||
        osver->dwOSVersionInfoSize != sizeof(OSVERSIONINFOEXA))
    {
        return FALSE;
    }

    // Win10
    osver->dwMajorVersion = 0x00000006;
    osver->dwMinorVersion = 0x00000002;
    osver->dwBuildNumber = 0x000023F0;
    osver->dwPlatformId = 0x00000002;
    osver->szCSDVersion[0] = 0;
    if (osver->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEXA))
    {
        LPOSVERSIONINFOEXA osverx = (LPOSVERSIONINFOEXA)osver;
        osverx->wServicePackMajor = 0x0000;
        osverx->wServicePackMinor = 0x0000;
        osverx->wSuiteMask = 0x0300;
        osverx->wProductType = 1;
        osverx->wReserved = 0;
    }
    return TRUE;
}

BOOL WINAPI
GetVersionExWHacked(LPOSVERSIONINFOW osver)
{
    if (osver->dwOSVersionInfoSize != sizeof(OSVERSIONINFOW) ||
        osver->dwOSVersionInfoSize != sizeof(OSVERSIONINFOEXW))
    {
        return FALSE;
    }

    // Win10
    osver->dwMajorVersion = 0x00000006;
    osver->dwMinorVersion = 0x00000002;
    osver->dwBuildNumber = 0x000023F0;
    osver->dwPlatformId = 0x00000002;
    osver->szCSDVersion[0] = 0;
    if (osver->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEXW))
    {
        LPOSVERSIONINFOEXW osverx = (LPOSVERSIONINFOEXW)osver;
        osverx->wServicePackMajor = 0x0000;
        osverx->wServicePackMinor = 0x0000;
        osverx->wSuiteMask = 0x0300;
        osverx->wProductType = 1;
        osverx->wReserved = 0;
    }
    return TRUE;
}

VOID WINAPI InitializeSRWLockForXP(PSRWLOCK_FOR_XP SRWLock)
{
    if (s_pInitializeSRWLock && DO_FALLBACK)
        return (*s_pInitializeSRWLock)(SRWLock);
    SRWLock->Ptr = LocalAlloc(LPTR, sizeof(CRITICAL_SECTION));
}
VOID WINAPI AcquireSRWLockExclusiveForXP(PSRWLOCK_FOR_XP SRWLock)
{
    if (s_pAcquireSRWLockExclusive && DO_FALLBACK)
    {
        (*s_pAcquireSRWLockExclusive)(SRWLock);
        return;
    }
    if (!SRWLock->Ptr)
        InitializeSRWLockForXP(SRWLock);
    EnterCriticalSection((CRITICAL_SECTION*)SRWLock->Ptr);
}
VOID WINAPI AcquireSRWLockSharedForXP(PSRWLOCK_FOR_XP SRWLock)
{
    if (s_pAcquireSRWLockShared && DO_FALLBACK)
    {
        (*s_pAcquireSRWLockShared)(SRWLock);
        return;
    }
    if (!SRWLock->Ptr)
        InitializeSRWLockForXP(SRWLock);
    EnterCriticalSection((CRITICAL_SECTION*)SRWLock->Ptr);
}
BOOLEAN WINAPI TryAcquireSRWLockExclusiveForXP(PSRWLOCK_FOR_XP SRWLock)
{
    if (s_pTryAcquireSRWLockExclusive && DO_FALLBACK)
        return (*s_pTryAcquireSRWLockExclusive)(SRWLock);
    if (!SRWLock->Ptr)
        InitializeSRWLockForXP(SRWLock);
    return TryEnterCriticalSection((CRITICAL_SECTION*)SRWLock->Ptr);
}
BOOLEAN WINAPI TryAcquireSRWLockSharedForXP(PSRWLOCK_FOR_XP SRWLock)
{
    if (s_pTryAcquireSRWLockShared && DO_FALLBACK)
        return (*s_pTryAcquireSRWLockShared)(SRWLock);
    if (!SRWLock->Ptr)
        InitializeSRWLockForXP(SRWLock);
    return TryEnterCriticalSection((CRITICAL_SECTION*)SRWLock->Ptr);
}
VOID WINAPI ReleaseSRWLockExclusiveForXP(PSRWLOCK_FOR_XP SRWLock)
{
    if (s_pReleaseSRWLockExclusive && DO_FALLBACK)
    {
        (*s_pReleaseSRWLockExclusive)(SRWLock);
        return;
    }
    if (!SRWLock->Ptr)
        return;
    LeaveCriticalSection((CRITICAL_SECTION*)SRWLock->Ptr);
}
VOID WINAPI ReleaseSRWLockSharedForXP(PSRWLOCK_FOR_XP SRWLock)
{
    if (s_pReleaseSRWLockShared && DO_FALLBACK)
    {
        (*s_pReleaseSRWLockShared)(SRWLock);
        return;
    }
    if (!SRWLock->Ptr)
        return;
    LeaveCriticalSection((CRITICAL_SECTION*)SRWLock->Ptr);
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
        s_pInitializeSRWLock = (FN_InitializeSRWLock)GetProcAddress(s_hKernel32, "InitializeSRWLock");
        s_pAcquireSRWLockExclusive = (FN_AcquireSRWLockExclusive)GetProcAddress(s_hKernel32, "AcquireSRWLockExclusive");
        s_pAcquireSRWLockShared = (FN_AcquireSRWLockShared)GetProcAddress(s_hKernel32, "AcquireSRWLockShared");
        s_pTryAcquireSRWLockExclusive = (FN_TryAcquireSRWLockExclusive)GetProcAddress(s_hKernel32, "TryAcquireSRWLockExclusive");
        s_pTryAcquireSRWLockShared = (FN_TryAcquireSRWLockShared)GetProcAddress(s_hKernel32, "TryAcquireSRWLockShared");
        s_pReleaseSRWLockExclusive = (FN_ReleaseSRWLockExclusive)GetProcAddress(s_hKernel32, "ReleaseSRWLockExclusive");
        s_pReleaseSRWLockShared = (FN_ReleaseSRWLockShared)GetProcAddress(s_hKernel32, "ReleaseSRWLockShared");
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
