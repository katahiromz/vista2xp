/* opendlg.c --- vista2xp testcase
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

    IFileOpenDialog *pFileOpen = NULL;
    IShellItem *pItem = NULL;
    PWSTR pszFilePath = NULL;

    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
                          IID_IFileOpenDialog,
                          reinterpret_cast<void**>(&pFileOpen));
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
