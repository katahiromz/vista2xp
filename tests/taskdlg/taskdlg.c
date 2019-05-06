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
    TaskDialog(NULL, NULL, L"Title", L"Title 2", L"This is a test for TaskDialog.",
               TDCBF_YES_BUTTON | TDCBF_NO_BUTTON | TDCBF_CANCEL_BUTTON,
               TD_SHIELD_ICON, &id);
    printf("id: %d\n", id);
    return 0;
}
