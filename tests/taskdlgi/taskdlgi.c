/* taskdlgi.c --- vista2xp testcase
 * This file is public domain software.
 * Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
 */

#include "targetvervista.h"
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

enum WEATHER
{
    FINE = 100, CLOUD, RAIN, SNOW
};

int main(void)
{
#if defined(__GNUC__) && (__GNUC__ < 7)
    printf("TaskDialogIndirect is not supported\n");
#else
    {
        TASKDIALOGCONFIG config = { sizeof(TASKDIALOGCONFIG) };
        config.hInstance = GetModuleHandleA(NULL);
        config.dwCommonButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
        config.pszWindowTitle = TEXT("WindowTitle");
        config.pszMainIcon = TD_SHIELD_ICON;
        config.pszMainInstruction = MAKEINTRESOURCEW(100);
        config.pszContent = TEXT("pszContent");
        INT selected = 0xDEADBEEF;
        HRESULT hr = TaskDialogIndirect(&config, &selected, NULL, NULL);
        printf("hr: %08X\n", hr);
        printf("selected: %d\n", selected);
    }
    {
        TASKDIALOG_BUTTON buttons[] =
        {
            { 100, L"&Download and install the update now" },
            { 101, L"Do &not download the update" }
        };
        TASKDIALOGCONFIG config = { sizeof(TASKDIALOGCONFIG) };
        config.hInstance = GetModuleHandleA(NULL);
        config.pButtons = buttons;
        config.cButtons = ARRAYSIZE(buttons);
        config.pszWindowTitle = TEXT("WindowTitle");
        config.pszMainIcon = TD_INFORMATION_ICON;
        config.pszMainInstruction = TEXT("pszMainInstruction");
        config.pszContent = TEXT("pszContent");
        config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_USE_COMMAND_LINKS;
        INT selected = 0xDEADBEEF;
        HRESULT hr = TaskDialogIndirect(&config, &selected, NULL, NULL);
        printf("hr: %08X\n", hr);
        printf("selected: %d\n", selected);
    }
    {
        TASKDIALOG_BUTTON buttons[] =
        {
            { 100, L"&Download and install the update now\n"
                   L"Update the program to version 2007.1" },
            { 101, L"Do &not download the update\n"
                   L"You will be reminded to install the update in one week" }
        };
        TASKDIALOGCONFIG config = { sizeof(TASKDIALOGCONFIG) };
        config.hInstance = GetModuleHandleA(NULL);
        config.pButtons = buttons;
        config.cButtons = ARRAYSIZE(buttons);
        config.pszWindowTitle = TEXT("WindowTitle");
        config.pszMainIcon = TD_INFORMATION_ICON;
        config.pszMainInstruction = TEXT("pszMainInstruction");
        config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_USE_COMMAND_LINKS;
        INT selected = 0xDEADBEEF;
        HRESULT hr = TaskDialogIndirect(&config, &selected, NULL, NULL);
        printf("hr: %08X\n", hr);
        printf("selected: %d\n", selected);
    }
    {
        TASKDIALOG_BUTTON buttons[] =
        {
            { FINE, TEXT("&Fine") },
            { CLOUD, TEXT("&Cloud") },
            { RAIN, TEXT("&Rain") },
            { SNOW, TEXT("&Snow") }
        };
        TASKDIALOGCONFIG config;
        ZeroMemory(&config, sizeof(config));
        config.cbSize = sizeof(config); 
        config.hInstance = GetModuleHandleA(NULL);
        config.hwndParent = NULL;
        config.pButtons = buttons;
        config.cButtons = ARRAYSIZE(buttons);
        config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
        config.pszMainIcon = TD_WARNING_ICON; 
        config.pszWindowTitle = TEXT("pszWindowTitle");
        config.pszMainInstruction = TEXT("pszMainInstruction"); 
        config.pszContent = TEXT("pszContent"); 
        config.pszFooterIcon = TD_INFORMATION_ICON;
        config.pszFooter = TEXT("pszFooter");
        config.nDefaultButton = CLOUD;
        INT selected = 0xDEADBEEF;
        HRESULT hr = TaskDialogIndirect(&config, &selected, NULL, NULL);
        printf("hr: %08X\n", hr);
        switch (selected)
        {
        case FINE:
            puts("FINE");
            break;
        case CLOUD:
            puts("CLOUD");
            break;
        case RAIN:
            puts("RAIN");
            break;
        case SNOW:
            puts("SNOW");
            break;
        }
    }
#endif

    return 0;
}
