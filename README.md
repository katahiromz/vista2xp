# vista2xp --- Vista-to-XP application converter

## What's this?

This software may be able to convert an app for Vista/7/10 to an XP app.
I'm sorry if unable to do it.

Platforms: Vista/7/10

Download: https://katahiromz.web.fc2.com/vista2xp/en

## How it works?

It modifies the IAT (Import Address Table) of EXE/DLL files.

- If any Vista+ `kernel32` functions, importing of `kernel32.dll` will be altered to `v2xker32.dll`.
- If any Vista+ `comctl32` functions, importing of `comctl32.dll` will be altered to `v2xctl32.dll`.
- If any Vista+ `user32` functions, importing of `user32.dll` will be altered to `v2xu32.dll`.

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
    - Add v2xctl32 and v2xu32.

## How to build?

Use MSYS2, and use cmake of MSYS2. Visual Studio can't build.

## Contact Us

Katayama Hirofumi MZ
katayama.hirofumi.mz@gmail.com
