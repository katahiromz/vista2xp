/* taskdlg.c --- vista2xp testcase
 * This file is public domain software.
 * Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
 */

/* Vista+ */
#define WINVER          0x0600
#define _WIN32_WINNT    0x0600
#define _WIN32_IE       0x0600
#define NTDDI_VERSION   0x06000000

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

int main(void)
{
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

    return 0;
}
