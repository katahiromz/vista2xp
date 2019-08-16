/* memcpy.c --- vista2xp testcase
 * This file is public domain software.
 * Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
 */

#include "targetvervista.h"
#include <windows.h>
#include <stdio.h>

int main(void)
{
    char buf[32];
    char sz[32] = "This is test.\n";
    memcpy_s(buf, sizeof(buf), sz, sizeof(sz));
    if (lstrcmpA(buf, sz) != 0)
    {
        puts("NG");
        return -1;
    }
    memmove_s(buf, sizeof(buf), sz, sizeof(sz));
    if (lstrcmpA(buf, sz) != 0)
    {
        puts("NG");
        return -2;
    }
    puts("OK");
    return 0;
}
