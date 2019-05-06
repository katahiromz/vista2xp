/* regtest.c --- vista2xp testcase
 * This file is public domain software.
 * Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
 */

/* Vista+ */
#define WINVER          0x0600
#define _WIN32_WINNT    0x0600
#define _WIN32_IE       0x0600
#define NTDDI_VERSION   0x06000000

#include <windows.h>
#include <stdio.h>

int main(void)
{
    HKEY hKey = NULL;
    RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer"), 0, KEY_READ, &hKey);
    if (hKey)
    {
        puts("OK");
        RegCloseKey(hKey);
        return 0;
    }
    puts("NG");
    return -1;
}
