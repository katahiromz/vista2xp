﻿(Japanese)

# vista2xp --- VistaからXPへのアプリ変換ツール

## これは何？

このソフトは、Vista/7/10向けのアプリをXP向けのアプリに変換できるかもしれないソフトです。
できなかったらごめんなさい。

対応環境: Vista/7/10

ダウンロード: https://katahiromz.web.fc2.com/vista2xp/ja

## どのように動作する？

これは、EXE/DLL ファイルの IAT (Import Address Table) を改変します。
kernel32.dll のインポートは、私が製造した v2xker32.dll へ変更されます。
v2xker32.dll ファイルが XP 用の回避策を行います。

## 使い方は？

1. vista2xp.exe プログラムファイルを開いて下さい。ダイアログ ボックスが開かれるでしょう。
2. 変換したい EXE/DLL ファイルをダイアログにドロップして下さい。
3. [変換]ボタンをクリックして下さい。
4. ファイルは変換されるでしょう。ファイル v2xker32.dll は、必要ならば自動で追加されます。
5. 元に戻したいなら、作成された「Vista2XP-Backup」フォルダ内部のファイルをお使い下さい。

## 連絡先

片山博文MZ (かたやまひろふみエムゼッド)

katayama.hirofumi.mz@gmail.com