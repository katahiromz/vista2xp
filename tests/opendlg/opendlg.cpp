/* opendlg.c --- vista2xp testcase
 * This file is public domain software.
 * Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
 */

#include "targetvervista.h"
#include <windows.h>
#include <initguid.h>
#include <shobjidl.h>
#include <stdio.h>

DEFINE_GUID(IID_IFileOpenDialog_, 0xd57c7288, 0xd4ad, 0x4768, 0xbe,0x02, 0x9d,0x96,0x95,0x32,0xd9,0x60);
DEFINE_GUID(CLSID_FileOpenDialog_, 0xdc1c5a9c, 0xe88a, 0x4dde, 0xa5,0xa1, 0x60,0xf8,0x2a,0x20,0xae,0xf7);

int main(void)
{
    int ret = -1;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        return ret;
    }

    IFileOpenDialog *pFileOpen = NULL;
    IShellItem *pItem = NULL;
    PWSTR pszFilePath = NULL;
    FILEOPENDIALOGOPTIONS fos = 0xFFFFFFFF;
    LPWSTR pszFileName = NULL;
    UINT iIndex = -3;
    IShellItem *psi = NULL;

    hr = CoCreateInstance(CLSID_FileOpenDialog_, NULL, CLSCTX_ALL, 
                          IID_IFileOpenDialog_,
                          reinterpret_cast<void**>(&pFileOpen));
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        goto cleanup;
    }

    hr = pFileOpen->GetOptions(&fos);
    printf("Line %d: %08X: %08X\n", __LINE__, hr, fos);

    hr = pFileOpen->GetFileName(&pszFileName);
    printf("Line %d: %08X: '%ls'\n", __LINE__, hr, pszFileName);
    CoTaskMemFree(pszFileName);

    hr = pFileOpen->GetFileTypeIndex(&iIndex);
    printf("Line %d: %08X: %u\n", __LINE__, hr, iIndex);

    hr = pFileOpen->GetCurrentSelection(&psi);
    printf("Line %d: %08X: %p\n", __LINE__, hr, psi);
    if (psi)
    {
        hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFileName);
        psi->Release();
        printf("Line %d: %08X: '%ls'\n", __LINE__, hr, pszFileName);
        CoTaskMemFree(pszFileName);
    }

    hr = pFileOpen->SetDefaultExtension(NULL);
    printf("Line %d: %08X\n", __LINE__, hr);

    hr = pFileOpen->SetDefaultExtension(L"txt");
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        goto cleanup;
    }

    hr = pFileOpen->Show(NULL);
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        goto cleanup;
    }

    hr = pFileOpen->GetResult(&pItem);
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        goto cleanup;
    }

    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        goto cleanup;
    }

    printf("File Path: %ls\n", pszFilePath);

cleanup:
    if (pItem)
        pItem->Release();
    if (pFileOpen)
        pFileOpen->Release();
    if (pszFilePath)
        CoTaskMemFree(pszFilePath);
    CoUninitialize();
    return ret;
}
