/* v2xker32.c --- v2xker32.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetverxp.h"
#include <windows.h>
#include <string.h>
#include <strsafe.h>
#include <psapi.h>
#include "WonFileID.h"

typedef struct tagSRWLOCK_FOR_XP
{
    PVOID Ptr;
} SRWLOCK_FOR_XP, *PSRWLOCK_FOR_XP;

typedef union tagINIT_ONCE_FOR_XP
{
    PVOID Ptr;
} INIT_ONCE_FOR_XP, *PINIT_ONCE_FOR_XP;

typedef BOOL (WINAPI *PINIT_ONCE_FN_FOR_XP)(PINIT_ONCE_FOR_XP, PVOID, PVOID *);

typedef struct tagCONDITION_VARIABLE_FOR_XP
{
    PVOID Ptr;
} CONDITION_VARIABLE_FOR_XP, *PCONDITION_VARIABLE_FOR_XP;

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

typedef VOID (WINAPI *FN_InitOnceInitialize)(PINIT_ONCE_FOR_XP);
typedef BOOL (WINAPI *FN_InitOnceExecuteOnce)(PINIT_ONCE_FOR_XP, PINIT_ONCE_FN_FOR_XP, PVOID, LPVOID *);

static FN_InitOnceInitialize s_pInitOnceInitialize;
static FN_InitOnceExecuteOnce s_pInitOnceExecuteOnce;

typedef VOID (WINAPI *FN_InitializeConditionVariable)(PCONDITION_VARIABLE_FOR_XP);
typedef BOOL (WINAPI *FN_SleepConditionVariableCS)(PCONDITION_VARIABLE_FOR_XP, PCRITICAL_SECTION, DWORD);
typedef BOOL (WINAPI *FN_SleepConditionVariableSRW)(PCONDITION_VARIABLE_FOR_XP, PSRWLOCK_FOR_XP, DWORD, ULONG);
typedef VOID (WINAPI *FN_WakeConditionVariable)(PCONDITION_VARIABLE_FOR_XP);
typedef VOID (WINAPI *FN_WakeAllConditionVariable)(PCONDITION_VARIABLE_FOR_XP);

static FN_InitializeConditionVariable s_pInitializeConditionVariable;
static FN_SleepConditionVariableCS s_pSleepConditionVariableCS;
static FN_SleepConditionVariableSRW s_pSleepConditionVariableSRW;
static FN_WakeConditionVariable s_pWakeConditionVariable;
static FN_WakeAllConditionVariable s_pWakeAllConditionVariable;

typedef LANGID (WINAPI *FN_GetThreadUILanguage)(VOID);
typedef LANGID (WINAPI *FN_SetThreadUILanguage)(LANGID);

static FN_GetThreadUILanguage s_pGetThreadUILanguage;
static FN_SetThreadUILanguage s_pSetThreadUILanguage;

typedef INT (WINAPI *FN_CompareStringEx)(LPCWSTR, DWORD, LPCWCH, INT, LPCWCH, INT, LPNLSVERSIONINFO, LPVOID, LPARAM);
static FN_CompareStringEx s_pCompareStringEx;

typedef HANDLE (WINAPI *FN_CreateEventExA)(LPSECURITY_ATTRIBUTES, LPCSTR, DWORD, DWORD);
typedef HANDLE (WINAPI *FN_CreateEventExW)(LPSECURITY_ATTRIBUTES, LPCWSTR, DWORD, DWORD);
static FN_CreateEventExA s_pCreateEventExA;
static FN_CreateEventExW s_pCreateEventExW;

typedef BOOL (WINAPI *FN_SetFileInformationByHandle)(
    HANDLE hFile,
    WON_FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
    LPVOID lpFileInformation,
    DWORD dwBufferSize);
typedef BOOL (WINAPI *FN_GetFileInformationByHandleEx)(
    HANDLE hFile,
    WON_FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
    LPVOID lpFileInformation,
    DWORD dwBufferSize);
typedef HANDLE (WINAPI *FN_OpenFileById)(
    HANDLE hVolumeHint,
    LPWON_FILE_ID_DESCRIPTOR lpFileId,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwFlagsAndAttributes);

FN_SetFileInformationByHandle s_pSetFileInformationByHandle;
FN_GetFileInformationByHandleEx s_pGetFileInformationByHandleEx;
FN_OpenFileById s_pOpenFileById;

BOOL WINAPI
IsWow64ProcessForXP(HANDLE hProcess, PBOOL Wow64Process)
{
    if (s_pIsWow64Process && DO_FALLBACK)
        return (*s_pIsWow64Process)(hProcess, Wow64Process);
    return FALSE;
}

BOOL IsWindowsVistaOrLater(VOID)
{
    OSVERSIONINFOW osver = { sizeof(osver) };
    return (GetVersionExW(&osver) && osver.dwMajorVersion >= 6);
}

LANGID GetThreadUILanguageForXP()
{
    if (IsWindowsVistaOrLater() && s_pGetThreadUILanguage)
        return (*s_pGetThreadUILanguage)();

    return LANGIDFROMLCID(GetThreadLocale());
}

LANGID SetThreadUILanguageForXP(LANGID LangID)
{
    if (IsWindowsVistaOrLater() && s_pSetThreadUILanguage)
        return (*s_pSetThreadUILanguage)(LangID);

    if (SetThreadLocale(MAKELCID(LangID, SORT_DEFAULT)))
        return LangID;

    return 0;
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

VOID WINAPI InitOnceInitializeForXP(PINIT_ONCE_FOR_XP InitOnce)
{
    if (s_pInitOnceInitialize && DO_FALLBACK)
    {
        (*s_pInitOnceInitialize)(InitOnce);
        return;
    }
    InitOnce->Ptr = NULL;
}

BOOL WINAPI
InitOnceExecuteOnceForXP(PINIT_ONCE_FOR_XP InitOnce, PINIT_ONCE_FN_FOR_XP InitFn,
                         PVOID Parameter, LPVOID *Context)
{
    if (s_pInitOnceExecuteOnce && DO_FALLBACK)
        return (*s_pInitOnceExecuteOnce)(InitOnce, InitFn, Parameter, Context);
    if (InterlockedExchange((LPLONG)&InitOnce->Ptr, (1 << INIT_ONCE_CTX_RESERVED_BITS) - 1) == 0)
    {
        (*InitFn)(InitOnce, Parameter, Context);
    }
}

typedef struct COND_VAR_THREAD
{
    struct COND_VAR_THREAD *next;
    HANDLE hEvent;
} COND_VAR_THREAD;

typedef struct COND_VAR
{
    DWORD dwTlsIndex;
    COND_VAR_THREAD *threads;
} COND_VAR;

VOID WINAPI
InitializeConditionVariableForXP(PCONDITION_VARIABLE_FOR_XP ConditionVariable)
{
    DWORD dwTlsIndex;
    COND_VAR *var;
    if (s_pInitializeConditionVariable && DO_FALLBACK)
        return (*s_pInitializeConditionVariable)(ConditionVariable);
    dwTlsIndex = TlsAlloc();
    var = LocalAlloc(LPTR, sizeof(COND_VAR));
    if (!var)
    {
        ConditionVariable->Ptr = NULL;
        return;
    }
    var->dwTlsIndex = dwTlsIndex;
    var->threads = NULL;
    ConditionVariable->Ptr = var;
}

static HANDLE CondVar_GetEventHandle(PCONDITION_VARIABLE_FOR_XP ConditionVariable)
{
    COND_VAR_THREAD *thread;
    COND_VAR *var = (COND_VAR *)ConditionVariable->Ptr;
    if (!var)
    {
        InitializeConditionVariableForXP(ConditionVariable);
        var = (COND_VAR *)ConditionVariable->Ptr;
        if (var == NULL)
            return NULL;
    }
    thread = TlsGetValue(var->dwTlsIndex);
    if (!thread)
    {
        thread = LocalAlloc(LPTR, sizeof(COND_VAR_THREAD));
        if (!thread)
            return NULL;
        TlsSetValue(var->dwTlsIndex, thread);
        if (!var->threads)
        {
            var->threads = thread;
        }
        else
        {
            thread->next = var->threads;
            var->threads = thread;
        }
    }
    if (!thread->hEvent)
        thread->hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    return (HANDLE)thread->hEvent;
}

BOOL WINAPI
SleepConditionVariableCSForXP(PCONDITION_VARIABLE_FOR_XP ConditionVariable,
                              PCRITICAL_SECTION CriticalSection, DWORD dwMilliseconds)
{
    HANDLE hEvent;
    if (s_pSleepConditionVariableCS && DO_FALLBACK)
        return (*s_pSleepConditionVariableCS)(ConditionVariable, CriticalSection, dwMilliseconds);
    hEvent = CondVar_GetEventHandle(ConditionVariable);
    WaitForSingleObject(hEvent, dwMilliseconds);
    LeaveCriticalSection(CriticalSection);
}

BOOL WINAPI
SleepConditionVariableSRWForXP(PCONDITION_VARIABLE_FOR_XP ConditionVariable,
                               PSRWLOCK_FOR_XP SRWLock,
                               DWORD dwMilliseconds, ULONG Flags)
{
    HANDLE hEvent;
    if (s_pSleepConditionVariableSRW && DO_FALLBACK)
        return (*s_pSleepConditionVariableSRW)(ConditionVariable, SRWLock, dwMilliseconds, Flags);
    hEvent = CondVar_GetEventHandle(ConditionVariable);
    WaitForSingleObject(hEvent, dwMilliseconds);
    if (Flags & CONDITION_VARIABLE_LOCKMODE_SHARED)
        ReleaseSRWLockSharedForXP(SRWLock);
    else
        ReleaseSRWLockExclusiveForXP(SRWLock);
}

VOID WINAPI WakeConditionVariableForXP(PCONDITION_VARIABLE_FOR_XP ConditionVariable)
{
    HANDLE hEvent;
    if (s_pWakeConditionVariable && DO_FALLBACK)
        return (*s_pWakeConditionVariable)(ConditionVariable);
    hEvent = CondVar_GetEventHandle(ConditionVariable);
    if (hEvent == NULL)
        return;
    SetEvent(hEvent);
}

VOID WINAPI WakeAllConditionVariableForXP(PCONDITION_VARIABLE_FOR_XP ConditionVariable)
{
    COND_VAR_THREAD *thread;
    COND_VAR *var;
    if (s_pWakeAllConditionVariable && DO_FALLBACK)
        return (*s_pWakeAllConditionVariable)(ConditionVariable);
    var = (COND_VAR *)ConditionVariable->Ptr;
    if (!var)
        return;
    thread = var->threads;
    while (thread)
    {
        SetEvent(thread->hEvent);
        thread = thread->next;
    }
}

INT WINAPI
CompareStringExForXP(
    LPCWSTR           lpLocaleName,
    DWORD             dwCmpFlags,
    LPCWCH            lpString1,
    INT               cchCount1,
    LPCWCH            lpString2,
    INT               cchCount2,
    LPNLSVERSIONINFO  lpVersionInformation,
    LPVOID            lpReserved,
    LPARAM            lParam)
{
    if (s_pCompareStringEx && DO_FALLBACK)
        return s_pCompareStringEx(lpLocaleName,
                                  dwCmpFlags,
                                  lpString1,
                                  cchCount1,
                                  lpString2,
                                  cchCount2,
                                  lpVersionInformation,
                                  lpReserved,
                                  lParam);

    dwCmpFlags &= (NORM_IGNORECASE |
                   NORM_IGNOREKANATYPE |
                   NORM_IGNORENONSPACE |
                   NORM_IGNORESYMBOLS |
                   NORM_IGNOREWIDTH |
                   SORT_STRINGSORT);

    return CompareStringW(LOCALE_USER_DEFAULT,
                          dwCmpFlags,
                          lpString1,
                          cchCount1,
                          lpString2,
                          cchCount2);
}

#ifndef CREATE_EVENT_MANUAL_RESET
    #define CREATE_EVENT_MANUAL_RESET   0x00000001
    #define CREATE_EVENT_INITIAL_SET    0x00000002
#endif

HANDLE WINAPI
CreateEventExAForXP(
    LPSECURITY_ATTRIBUTES lpEventAttributes,
    LPCSTR                lpName,
    DWORD                 dwFlags,
    DWORD                 dwDesiredAccess)
{
    BOOL bManualReset, bInitialSet;

    if (s_pCreateEventExA && DO_FALLBACK)
        return s_pCreateEventExA(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);

    bManualReset = !!(dwFlags & CREATE_EVENT_MANUAL_RESET);
    bInitialSet = !!(dwFlags & CREATE_EVENT_INITIAL_SET);

    return CreateEventA(lpEventAttributes, bManualReset, bInitialSet, lpName);
}

HANDLE WINAPI
CreateEventExWForXP(
    LPSECURITY_ATTRIBUTES lpEventAttributes,
    LPCWSTR               lpName,
    DWORD                 dwFlags,
    DWORD                 dwDesiredAccess)
{
    BOOL bManualReset, bInitialSet;

    if (s_pCreateEventExW && DO_FALLBACK)
        return s_pCreateEventExW(lpEventAttributes, lpName, dwFlags, dwDesiredAccess);

    bManualReset = !!(dwFlags & CREATE_EVENT_MANUAL_RESET);
    bInitialSet = !!(dwFlags & CREATE_EVENT_INITIAL_SET);

    return CreateEventW(lpEventAttributes, bManualReset, bInitialSet, lpName);
}

BOOL WINAPI
SetFileInformationByHandleForXP(
    HANDLE hFile,
    WON_FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
    LPVOID lpFileInformation,
    DWORD dwBufferSize)
{
    if (s_pSetFileInformationByHandle && DO_FALLBACK)
    {
        return s_pSetFileInformationByHandle(hFile,
                                             FileInformationClass,
                                             lpFileInformation,
                                             dwBufferSize);
    }
    return WonSetFileInformationByHandle(hFile,
                                         FileInformationClass,
                                         lpFileInformation,
                                         dwBufferSize);
}

BOOL WINAPI
GetFileInformationByHandleExForXP(
    HANDLE hFile,
    WON_FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
    LPVOID lpFileInformation,
    DWORD dwBufferSize)
{
    if (s_pGetFileInformationByHandleEx && DO_FALLBACK)
    {
        return s_pGetFileInformationByHandleEx(hFile,
                                               FileInformationClass,
                                               lpFileInformation,
                                               dwBufferSize);
    }
    return WonGetFileInformationByHandleEx(hFile,
                                           FileInformationClass,
                                           lpFileInformation,
                                           dwBufferSize);
}

HANDLE WINAPI
OpenFileByIdForXP(
    HANDLE hVolumeHint,
    LPWON_FILE_ID_DESCRIPTOR lpFileId,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwFlagsAndAttributes)
{
    if (s_pOpenFileById && DO_FALLBACK)
    {
        return s_pOpenFileById(hVolumeHint,
                               lpFileId,
                               dwDesiredAccess,
                               dwShareMode,
                               lpSecurityAttributes,
                               dwFlagsAndAttributes);
    }
    return WonOpenFileById(hVolumeHint,
                           lpFileId,
                           dwDesiredAccess,
                           dwShareMode,
                           lpSecurityAttributes,
                           dwFlagsAndAttributes);
}

#define GETPROC(fn) s_p##fn = (FN_##fn)GetProcAddress(s_hKernel32, #fn)

BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        s_hinstDLL = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
        s_bUseQPC = QueryPerformanceFrequency(&s_freq);
        s_hKernel32 = LoadLibraryA("kernel32");
        GETPROC(IsWow64Process);
        GETPROC(GetTickCount64);
        GETPROC(QueryFullProcessImageNameA);
        GETPROC(QueryFullProcessImageNameW);
        GETPROC(InitializeSRWLock);
        GETPROC(AcquireSRWLockExclusive);
        GETPROC(AcquireSRWLockShared);
        GETPROC(TryAcquireSRWLockExclusive);
        GETPROC(TryAcquireSRWLockShared);
        GETPROC(ReleaseSRWLockExclusive);
        GETPROC(ReleaseSRWLockShared);
        GETPROC(InitOnceInitialize);
        GETPROC(InitOnceExecuteOnce);
        GETPROC(InitializeConditionVariable);
        GETPROC(SleepConditionVariableCS);
        GETPROC(SleepConditionVariableSRW);
        GETPROC(WakeConditionVariable);
        GETPROC(WakeAllConditionVariable);
        GETPROC(GetThreadUILanguage);
        GETPROC(SetThreadUILanguage);
        GETPROC(CompareStringEx);
        GETPROC(CreateEventExA);
        GETPROC(CreateEventExW);
        GETPROC(SetFileInformationByHandle);
        GETPROC(GetFileInformationByHandleEx);
        GETPROC(OpenFileById);
        break;
    case DLL_PROCESS_DETACH:
        FreeLibrary(s_hKernel32);
        break;
    }
    return TRUE;
}
