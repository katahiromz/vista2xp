/* taskdlg.c --- vista2xp testcase
 * This file is public domain software.
 * Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
 */

#include "targetvervista.h"
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

int main(void)
{
#if defined(__GNUC__) && (__GNUC__ < 7)
    printf("TaskDialog is not supported\n");
#else
    INT id;
    InitCommonControls();

    TaskDialog(NULL, NULL, L"Test #1", L"Sheild",
               L"This is a test for TaskDialog.",
               TDCBF_YES_BUTTON | TDCBF_NO_BUTTON | TDCBF_CANCEL_BUTTON,
               TD_SHIELD_ICON, &id);
    printf("id: %d\n", id);

    TaskDialog(NULL, GetModuleHandleA(NULL), L"Test #2", L"Error",
               MAKEINTRESOURCEW(100),
               TDCBF_YES_BUTTON | TDCBF_NO_BUTTON,
               TD_ERROR_ICON, &id);
    printf("id: %d\n", id);

    TaskDialog(NULL, GetModuleHandleA(NULL), MAKEINTRESOURCEW(102), L"Warning",
               L"This is a test for TaskDialog.",
               TDCBF_RETRY_BUTTON | TDCBF_CANCEL_BUTTON,
               TD_WARNING_ICON, &id);
    printf("id: %d\n", id);

    TaskDialog(NULL, GetModuleHandleA(NULL), L"Test #4", MAKEINTRESOURCEW(101),
               L"This is a test for TaskDialog.",
               TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON,
               TD_INFORMATION_ICON, &id);
    printf("id: %d\n", id);
#endif

    return 0;
}
