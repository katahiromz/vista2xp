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

Visual Studio コマンドプロンプトの dumpbin /imports で IAT をチェックできます。

## 使い方は？

1. vista2xp.exe プログラムファイルを開いて下さい。ダイアログ ボックスが開かれるでしょう。
2. 変換したい EXE/DLL ファイルをダイアログにドロップして下さい。
3. [変換]ボタンをクリックして下さい。
4. ファイルは変換されるでしょう。いくつかの DLL ファイルは、必要ならば自動で追加されます。
5. 元に戻したいなら、作成された「Vista2XP-Backup」フォルダ内部のファイルをお使い下さい。

## 履歴

- 2019.05.05 ver.0.2
    - 初公開。
- 2019.05.06 ver.0.3
    - v2xctl32 と v2xu32 を追加。
- 2019.05.09 ver.0.4
    - K32* 関数をサポート。
    - TaskDialog を改良。
    - v2xol (ole32) を追加。
    - v2xsh32 (shell32) を追加。
- 2019.XX.YY ver.0.5
    - [キャンセル] ボタンを [終了] にする。

## 警告

- システムファイルを変換しないで下さい。
- 圧縮ファイルに使用してもあまり意味がありません。展開後・インストール後のファイルについてお使い下さい。
- ドットネットと64ビットには未対応です。

## 連絡先

片山博文MZ (かたやまひろふみエムゼッド)

katayama.hirofumi.mz@gmail.com
