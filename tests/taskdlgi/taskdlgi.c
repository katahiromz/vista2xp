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
        HRESULT hr = TaskDialogIndirect(NULL, NULL, NULL, NULL);
        printf("hr: %08X\n", hr);
    }
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
        config.dwFlags = TDF_USE_COMMAND_LINKS;
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
    {
        TASKDIALOG_BUTTON buttons[] =
        {
            { FINE, TEXT("&Fine") },
            { CLOUD, TEXT("&Cloud") },
            { RAIN, TEXT("&Rain") },
            { SNOW, TEXT("&Snow") }
        };
        TASKDIALOG_BUTTON radio_buttons[] =
        {
            { 1000, TEXT("Radio 1") },
            { 1001, TEXT("Radio 2") },
            { 1002, TEXT("Radio 3") },
            { 1003, TEXT("Radio 4") },
            { 1004, TEXT("Radio 5") }
        };
        TASKDIALOGCONFIG config;
        ZeroMemory(&config, sizeof(config));
        config.cbSize = sizeof(config); 
        config.hInstance = GetModuleHandleA(NULL);
        config.hwndParent = NULL;
        config.pButtons = buttons;
        config.cButtons = ARRAYSIZE(buttons);
        config.pRadioButtons = radio_buttons;
        config.cRadioButtons = ARRAYSIZE(radio_buttons);
        config.nDefaultRadioButton = 1001;
        config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
        config.pszMainIcon = TD_WARNING_ICON; 
        config.pszWindowTitle = TEXT("with radio buttons");
        config.pszMainInstruction = TEXT("pszMainInstruction"); 
        config.pszContent = TEXT("pszContent"); 
        config.pszFooterIcon = TD_INFORMATION_ICON;
        config.pszFooter = TEXT("pszFooter");
        config.nDefaultButton = CLOUD;
        INT selected = 0xDEADBEEF;
        INT radio = 0xDEADBEEF;
        HRESULT hr = TaskDialogIndirect(&config, &selected, &radio, NULL);
        printf("hr: %08X\n", hr);
        printf("radio: %08X\n", radio);
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
    {
        TASKDIALOG_BUTTON buttons[] =
        {
            { FINE, TEXT("&Fine") },
            { CLOUD, TEXT("&Cloud") },
            { RAIN, TEXT("&Rain") },
            { SNOW, TEXT("&Snow") }
        };
        TASKDIALOG_BUTTON radio_buttons[] =
        {
            { 1000, TEXT("Radio 1") },
            { 1001, TEXT("Radio 2") },
            { 1002, TEXT("Radio 3") },
            { 1003, TEXT("Radio 4") },
            { 1004, TEXT("Radio 5") }
        };
        TASKDIALOGCONFIG config;
        ZeroMemory(&config, sizeof(config));
        config.cbSize = sizeof(config); 
        config.hInstance = GetModuleHandleA(NULL);
        config.hwndParent = NULL;
        config.pButtons = buttons;
        config.cButtons = ARRAYSIZE(buttons);
        config.pRadioButtons = radio_buttons;
        config.cRadioButtons = ARRAYSIZE(radio_buttons);
        config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_USE_COMMAND_LINKS | TDF_NO_DEFAULT_RADIO_BUTTON;
        config.pszMainIcon = TD_WARNING_ICON; 
        config.pszWindowTitle = TEXT("with radio buttons 2");
        config.pszMainInstruction = TEXT("pszMainInstruction"); 
        config.pszContent = TEXT("pszContent"); 
        config.pszFooterIcon = TD_INFORMATION_ICON;
        config.pszFooter = TEXT("pszFooter");
        config.nDefaultButton = CLOUD;
        INT selected = 0xDEADBEEF;
        INT radio = 0xDEADBEEF;
        HRESULT hr = TaskDialogIndirect(&config, &selected, &radio, NULL);
        printf("hr: %08X\n", hr);
        printf("radio: %08X\n", radio);
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
