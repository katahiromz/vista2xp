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
#include <cstdio>
#include <cassert>

#ifndef ARRAYSIZE
    #define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

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
    assert(hr == 0x80070057);
    hr = pFileSave->SetFileTypes(0, filterSpec);
    assert(hr == S_OK);
    hr = pFileSave->SetFileTypes(ARRAYSIZE(filterSpec), NULL);
    assert(hr == 0x80070057);
    hr = pFileSave->SetFileTypes(ARRAYSIZE(filterSpec), filterSpec);
    assert(hr == S_OK);

    hr = pFileSave->GetOptions(NULL);
    assert(hr == 0x80070057);

    hr = pFileSave->GetOptions(&fos);
    assert(hr == S_OK);
    assert(fos == 0x0000880A);

    hr = pFileSave->GetFileName(NULL);
    assert(hr == 0x80070057);

    hr = pFileSave->GetFileName(&pszFileName);
    assert(hr == 0x80004005);
    assert(pszFileName == NULL);
    CoTaskMemFree(pszFileName);

    hr = pFileSave->SetFileName(L"C:\\Test.txt");
    assert(hr == S_OK);

    hr = pFileSave->GetFileTypeIndex(NULL);
    assert(hr == 0x80070057);

    hr = pFileSave->GetFileTypeIndex(&iIndex);
    assert(hr == S_OK);
    assert(iIndex == 0);

    hr = pFileSave->GetCurrentSelection(NULL);
    assert(hr == 0x80070057);

    hr = pFileSave->GetCurrentSelection(&psi);
    assert(hr == 0x80004005);
    assert(psi == NULL);

    hr = pFileSave->SetDefaultExtension(NULL);
    assert(hr == S_OK);

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
