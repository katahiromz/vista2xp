# vista2xp --- Vista-to-XP application converter

## What's this?

This software may be able to convert an app for Vista/7/10 to an XP app.
I'm sorry if unable to do it.

Platforms: Windows XP/Vista/7/10

Download: https://katahiromz.web.fc2.com/vista2xp/en

## How it works?

It modifies the IAT (Import Address Table) of EXE/DLL files.

- If any Vista+ `kernel32` functions, importing of `kernel32.dll` will be altered to `v2xker32.dll`.
- If any Vista+ `comctl32` functions, importing of `comctl32.dll` will be altered to `v2xctl32.dll`.
- If any Vista+ `user32` functions, importing of `user32.dll` will be altered to `v2xu32.dll`.
- If any Vista+ `ole32` functions, importing of `ole32.dll` will be altered to `v2xol.dll`.
- If any Vista+ `shell32` functions, importing of `shell32.dll` will be altered to `v2xsh32.dll`.
- If any Vista+ `msvcrt` functions, importing of `msvcrt.dll` will be altered to `v2xcrt.dll`.
- If any Vista+ `advapi32` functions, importing of `advapi32.dll` will be altered to `v2xadv32.dll`.

You can check the IAT by `dumpbin /imports` of Visual Studio Command Prompt.

## How to use?

1. Open `vista2xp.exe` program file. A dialog box will be open.
2. Drop the EXE/DLL files to the dialog to be converted.
3. Click the `[Convert]` button.
4. Files will be converted. Some DLL files will be automatically added if necessary.
5. If you want to revert, please use the files inside the `Vista2XP-Backup` folder that was created.

## History

- 2019.05.05 ver.0.2
    - First release.
- 2019.05.06 ver.0.3
    - Add `v2xctl32` and `v2xu32`.
- 2019.05.09 ver.0.4
    - Support `K32*` functions.
    - Improve TaskDialog.
    - Add `v2xol` (`ole32`).
    - Add `v2xsh32` (`shell32`).
- 2019.05.22 ver.0.5
    - Make [Cancel] button [Exit].
    - Supported `SHGetLocalizedName`, `SHSetFolderPathA` and `SHSetFolderPathW`.
    - Supported `TaskDialogIndirect`.
- 2019.08.16 ver.0.6
    - Fixed broken v2xu32.dll.
    - Supported `SHCreateItemFromParsingName`.
    - Supported `GetDpiForWindow`.
    - Supported `SetThreadDpiAwarenessContext`.
    - Add `v2xcrt` (`msvcrt`).
    - Supported `wcsnlen`, `memmove_s` and `memcpy_s`.
    - Supported `__CxxFrameHandler3` and `_except_handler4_common`.
- 2019.09.15 ver.0.7
    - Modify optional header's version info.
    - Supported `GetVersion`, `GetVersionExA` and `GetVersionExW`.
- 2021.05.11 ver.0.8
    - SRW lock functions are partially supported.
    - Initialize-once functions are partially supported.
    - Condition variables functions are partially supported.
- 2022.01.23 ver.0.8.1
    - Supported GetThreadUILanguage/SetThreadUILanguage.
    - Improved v2xctl32 ordinal exports.
- 2024.06.02 ver.0.8.2
    - Added v2xadv32 (advapi32).
- 2024.06.03 ver.0.8.3
    - Supported CompareStringEx.
- 2024.XX.YY ver.0.8.4
    - Fixed fallbacking of v2xadv32.
    - Fixed loading DLLs.

## How to build?

Use ReactOS Build Environment, and use cmake. Visual Studio can't build.

## WARNING!

- Please don't convert the system files.
- It doesn't make sense to use on the compressed files. Please use on the installed/expanded files.
- DOTNET, DirectX 10 and x64 are not supported yet.

If you are lacking DirectX 10 support, take a look of this: [https://km-software-directx-10.apponic.com/](https://km-software-directx-10.apponic.com/).

## Contact Us

Katayama Hirofumi MZ
katayama.hirofumi.mz@gmail.com
