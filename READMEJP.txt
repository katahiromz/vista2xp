(Japanese)

# vista2xp --- VistaからXPへのアプリ変換ツール

## これは何？

このソフトは、Vista/7/10向けのアプリをXP向けのアプリに変換できるかもしれないソフトです。
できなかったらごめんなさい。

対応環境: Vista/7/10

ダウンロード: https://katahiromz.web.fc2.com/vista2xp/ja

## どのように動作する？

これは、EXE/DLL ファイルの IAT (Import Address Table) を改変します。

- Vista以降の kernel32 関数があれば、kernel32.dll のインポートは v2xker32.dll に変わります。
- Vista以降の comctl32 関数があれば、comctl32.dll のインポートは v2xctl32.dll に変わります。
- Vista以降の user32 関数があれば、user32.dll のインポートは v2xu32.dll に変わります。

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

## 連絡先

片山博文MZ (かたやまひろふみエムゼッド)

katayama.hirofumi.mz@gmail.com
