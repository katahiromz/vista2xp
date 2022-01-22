/* v2xol.cpp --- v2xol.dll for vista2xp */
/* This file is public domain software.
   Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>. */
#include "targetverxp.h"
#include <initguid.h>
#include <windows.h>
#include <objbase.h>
#include <strsafe.h>
#include "FileDialog.hpp"

extern const GUID CLSID_FileOpenDialog_;
extern const GUID CLSID_FileSaveDialog_;

static HINSTANCE s_hinstDLL;
static HINSTANCE s_hOle32;

// CoCreateInstance
typedef HRESULT (STDAPICALLTYPE *FN_CoCreateInstance)
    (REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID *);
static FN_CoCreateInstance s_pCoCreateInstance = NULL;

extern "C"
HRESULT STDAPICALLTYPE
CoCreateInstanceForXP(REFCLSID rclsid, LPUNKNOWN pUnkOuter,
                      DWORD dwClsContext, REFIID riid, LPVOID *ppv)
{
    HRESULT hr;

    if (!s_pCoCreateInstance)
        return E_UNEXPECTED;

    if (IsEqualIID(rclsid, CLSID_FileOpenDialog_) ||
        IsEqualIID(rclsid, CLSID_FileSaveDialog_))
    {
        if (DO_FALLBACK)
        {
            hr = (*s_pCoCreateInstance)(rclsid, pUnkOuter, dwClsContext, riid, ppv);
            if (SUCCEEDED(hr))
                return hr;
        }

        if (pUnkOuter)
            return CLASS_E_NOAGGREGATION;

        if (IsEqualIID(rclsid, CLSID_FileOpenDialog_))
        {
            IFileOpenDialog *pObj = createFileOpenDialog();
            if (!pObj)
                return E_OUTOFMEMORY;
            hr = pObj->QueryInterface(riid, ppv);
            pObj->Release();
            return hr;
        }

        if (IsEqualIID(rclsid, CLSID_FileSaveDialog_))
        {
            IFileSaveDialog *pObj = createFileSaveDialog();
            if (!pObj)
                return E_OUTOFMEMORY;
            hr = pObj->QueryInterface(riid, ppv);
            pObj->Release();
            return hr;
        }

        return REGDB_E_CLASSNOTREG;
    }
    else
    {
        hr = (*s_pCoCreateInstance)(rclsid, pUnkOuter, dwClsContext, riid, ppv);
        return hr;
    }
}

BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        s_hinstDLL = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
        s_hOle32 = GetModuleHandleA("ole32");
        s_pCoCreateInstance = (FN_CoCreateInstance)GetProcAddress(s_hOle32, "CoCreateInstance");
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
