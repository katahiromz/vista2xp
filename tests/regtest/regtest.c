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
