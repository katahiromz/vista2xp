(Japanese)

# vista2xp --- VistaからXPへのアプリ変換ツール

## これは何？

このソフトは、Vista/7/10向けのアプリをXP向けのアプリに変換できるかもしれないソフトです。
できなかったらごめんなさい。

対応環境: Windows XP/Vista/7/10

ダウンロード: https://katahiromz.web.fc2.com/vista2xp/ja

## どのように動作する？

これは、EXE/DLL ファイルの IAT (Import Address Table) を改変します。

- Vista以降の kernel32 関数があれば、kernel32.dll のインポートは v2xker32.dll に変わります。
- Vista以降の comctl32 関数があれば、comctl32.dll のインポートは v2xctl32.dll に変わります。
- Vista以降の user32 関数があれば、user32.dll のインポートは v2xu32.dll に変わります。
- Vista以降の ole32 関数があれば、ole32.dll のインポートは v2xol.dll に変わります。
- Vista以降の shell32 関数があれば、shell32.dll のインポートは v2xsh32.dll に変わります。
- Vista以降の msvcrt 関数があれば、msvcrt.dll のインポートは v2xcrt.dll に変わります。
- Vista以降の advapi32 関数があれば、advapi32.dll のインポートは v2xadv32.dll に変わります。

Visual Studio コマンドプロンプトの dumpbin /imports で IAT をチェックできます。

## 使い方は？

1. vista2xp.exe プログラムファイルを開いて下さい。ダイアログ ボックスが開かれるでしょう。
2. 変換したい EXE/DLL ファイルをダイアログにドロップして下さい。
3. [変換]ボタンをクリックして下さい。
4. ファイルは変換されるでしょう。いくつかの DLL ファイルは、必要ならば自動で追加されます。
5. 元に戻したいなら、作成された「Vista2XP-Backup」フォルダ内部のファイルをお使い下さい。

## 履歴

- 2019-05-05 ver.0.2
    - 初公開。
- 2019-05-06 ver.0.3
    - v2xctl32 と v2xu32 を追加。
- 2019-05-09 ver.0.4
    - K32* 関数をサポート。
    - TaskDialog を改良。
    - v2xol (ole32) を追加。
    - v2xsh32 (shell32) を追加。
- 2019-05-22 ver.0.5
    - [キャンセル] ボタンを [終了] にする。
    - SHGetLocalizedName、SHSetFolderPathA および SHSetFolderPathW 関数をサポート。
    - TaskDialogIndirect 関数をサポート。
- 2019-08-16 ver.0.6
    - 壊れた v2xu32.dll を修正。
    - SHCreateItemFromParsingName 関数をサポート。
    - GetDpiForWindow 関数をサポート。
    - SetThreadDpiAwarenessContext 関数をサポート。
    - v2xcrt (msvcrt) を追加。
    - wcsnlen と memmove_s と memcpy_s 関数をサポート。
    - __CxxFrameHandler3 と _except_handler4_common 関数をサポート。
- 2019-09-15 ver.0.7
    - オプショナル ヘッダーのバージョン情報を改変。
    - GetVersion、GetVersionExA、GetVersionExW 関数をサポート。
- 2021-05-11 ver.0.8
    - SRWロック関数を部分的にサポート。
    - Initialize-once関数を部分的にサポート。
    - 状態変数関数を部分的にサポート。
- 2022-01-23 ver.0.8.1
    - GetThreadUILanguage/SetThreadUILanguageをサポート。
    - v2xctl32 序数エクスポートを改良。
- 2024-06-02 ver.0.8.2
    - v2xadv32 (advapi32) を追加。
- 2024-06-03 ver.0.8.3
    - CompareStringEx のサポート。
- 2024-06-03 ver.0.8.4
    - v2xadv32 のフォールバックを修正。
    - DLLの読み込みを修正。
- 2024-06-06 ver.0.8.5
    - RegDeleteKeyExA/W のサポート。
    - CreateEventExA/W のサポート。
- 2024-06-07 ver.0.8.6
    - v2xadv32 のフックを修正。
    - SetFileInformationByHandle、GetFileInformationByHandleEx および OpenFileById のサポート。

## 警告

- システムファイルを変換しないで下さい。
- 圧縮ファイルに使用してもあまり意味がありません。展開後・インストール後のファイルについてお使い下さい。
- ドットネットとDirectX 10と64ビットには未対応です。

もしDirectX 10のサポートが足りなければ、次のページを参考にして下さい：https://km-software-directx-10.apponic.com/

## 連絡先

片山博文MZ (かたやまひろふみエムゼッド)

katayama.hirofumi.mz@gmail.com
