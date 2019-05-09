/* regtest.c --- vista2xp testcase
 * This file is public domain software.
 * Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
 */

#include "targetvervista.h"
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
