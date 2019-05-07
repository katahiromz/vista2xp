/* savedlg.c --- vista2xp testcase
 * This file is public domain software.
 * Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
 */

/* Vista+ */
#define WINVER          0x0600
#define _WIN32_WINNT    0x0600
#define _WIN32_IE       0x0600
#define NTDDI_VERSION   0x06000000

#include <windows.h>
#include <shobjidl.h>
#include <stdio.h>

int main(void)
{
    int ret = -1;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        return ret;
    }

    IFileSaveDialog *pFileSave = NULL;
    IShellItem *pItem = NULL;
    PWSTR pszFilePath = NULL;
    FILEOPENDIALOGOPTIONS fos = 0xFFFFFFFF;
    LPWSTR pszFileName = NULL;
    UINT iIndex = -3;
    IShellItem *psi = NULL;

    static const COMDLG_FILTERSPEC filterSpec[] =
    {
        { L"Text Files (*.txt)", L"*.txt"},
        { L"All Files (*.*)", L"*.*"}
    };

    hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, 
                          IID_IFileSaveDialog,
                          reinterpret_cast<void**>(&pFileSave));
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        goto cleanup;
    }

    hr = pFileSave->SetFileTypes(0, NULL);
    printf("Line %d: %08X\n", __LINE__, hr);
    hr = pFileSave->SetFileTypes(0, filterSpec);
    printf("Line %d: %08X\n", __LINE__, hr);
    hr = pFileSave->SetFileTypes(ARRAYSIZE(filterSpec), NULL);
    printf("Line %d: %08X\n", __LINE__, hr);

    hr = pFileSave->SetFileTypes(ARRAYSIZE(filterSpec), filterSpec);
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        goto cleanup;
    }

    hr = pFileSave->GetOptions(NULL);
    printf("Line %d: %08X\n", __LINE__, hr);

    hr = pFileSave->GetOptions(&fos);
    printf("Line %d: %08X: %08X\n", __LINE__, hr, fos);

    hr = pFileSave->GetFileName(NULL);
    printf("Line %d: %08X\n", __LINE__, hr);

    hr = pFileSave->GetFileName(&pszFileName);
    printf("Line %d: %08X: '%ls'\n", __LINE__, hr, pszFileName);
    CoTaskMemFree(pszFileName);

    hr = pFileSave->GetFileTypeIndex(NULL);
    printf("Line %d: %08X\n", __LINE__, hr);

    hr = pFileSave->GetFileTypeIndex(&iIndex);
    printf("Line %d: %08X: %u\n", __LINE__, hr, iIndex);

    hr = pFileSave->GetCurrentSelection(NULL);
    printf("Line %d: %08X\n", __LINE__, hr);

    hr = pFileSave->GetCurrentSelection(&psi);
    printf("Line %d: %08X: %p\n", __LINE__, hr, psi);
    if (psi)
    {
        hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFileName);
        psi->Release();
        printf("Line %d: %08X: '%ls'\n", __LINE__, hr, pszFileName);
        CoTaskMemFree(pszFileName);
    }

    hr = pFileSave->SetDefaultExtension(NULL);
    printf("Line %d: %08X\n", __LINE__, hr);

    hr = pFileSave->SetDefaultExtension(L"txt");
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        goto cleanup;
    }

    hr = pFileSave->Show(NULL);
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        goto cleanup;
    }

    hr = pFileSave->GetResult(&pItem);
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
    if (pFileSave)
        pFileSave->Release();
    if (pszFilePath)
        CoTaskMemFree(pszFilePath);
    CoUninitialize();
    return ret;
}
