/* multifile.c --- vista2xp testcase
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
    PWSTR pszFilePath = NULL;
    FILEOPENDIALOGOPTIONS fos = 0xFFFFFFFF;
    LPWSTR pszFileName = NULL;
    UINT iIndex = -3;
    IShellItem *psi = NULL;
    IShellItemArray *pArray = NULL;
    DWORD i, dwCount;

    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
                          IID_IFileOpenDialog,
                          reinterpret_cast<void**>(&pFileOpen));
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        goto cleanup;
    }

    hr = pFileOpen->GetOptions(&fos);
    printf("Line %d: %08X: %08X\n", __LINE__, hr, fos);

    hr = pFileOpen->SetOptions(fos | FOS_ALLOWMULTISELECT);
    printf("Line %d: %08X: %08X\n", __LINE__, hr, fos);

    hr = pFileOpen->Show(NULL);
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        goto cleanup;
    }

    hr = pFileOpen->GetResults(&pArray);
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        goto cleanup;
    }

    hr = pArray->GetCount(&dwCount);
    if (FAILED(hr))
    {
        printf("Line %d: FAILED:%08X\n", __LINE__, hr);
        goto cleanup;
    }

    for (i = 0; i < dwCount; ++i)
    {
        hr = pArray->GetItemAt(i, &psi);
        if (FAILED(hr))
        {
            printf("Line %d: FAILED:%08X\n", __LINE__, hr);
            goto cleanup;
        }
        hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
        if (FAILED(hr))
        {
            printf("Line %d: FAILED:%08X\n", __LINE__, hr);
            psi->Release();
            goto cleanup;
        }
        printf("File Path #%lu: %ls\n", i, pszFilePath);
        psi->Release();
        if (pszFilePath)
        {
            CoTaskMemFree(pszFilePath);
            pszFilePath = NULL;
        }
    }

cleanup:
    if (pArray)
        pArray->Release();
    if (pFileOpen)
        pFileOpen->Release();
    if (pszFilePath)
        CoTaskMemFree(pszFilePath);
    CoUninitialize();
    return ret;
}
