========================================================================
       DocuWorks 6.0, DocuWorks 5.0用プラグイン : SimplePageExtractor v.1.2.2
========================================================================

　これは、富士ゼロックス株式会社のDocuWorks 8.0 に機能を追加するプラグインソフトウェアです。このソフトウェア単独では動作しません。

1. 概要

　DocuWorksで整理しておいた文書から必要なページを抜き出してプレゼン資料を作成する、というのはDocuWorksの正しい使いかたです。ところが、対象となるページが多くなると抜き出す作業が面倒だ、というわけでできたのがこのプラグインです。
　付箋で印をつけておくと、その場所で切り分けてその付箋に書かれた名前のDW文書に作ります。

2. インストール

　このプラグインはDocuWorks 8.0で利用できます。DocuWorks 5.0用のプログラムをDocuWorks Development Tool Kit 8 を使ってビルドしなおしたものですので、DocuWorks 7.x 等でも利用可能と思われます。
  このソフトウェアは、富士ゼロックス株式会社の「DocuWorks(TM) Development Tool Kit 5.0.6」、「DocuWorks(TM) Development Tool Kit 8」を用いて作成されています。

　SimplePageExtractor.dll を <DocuWorksインストールディレクトリ>\bin\plugins\ ( 通常は、C:\Program Files (x86)\Fuji Xerox\DocuWorks\bin\plugins\ )の下にコピーします。　

  このプラグインを使用するには、DocuWorks Deskを起動します。次に、DocuWorks Deskの[ツール]>[ツールの設定...]メニューでツールバーの設定ダイアログを開き、分類でツールを選ぶとコマンドに表示される「簡易ページ収集」をDocuWorks Deskのツールバーにドラッグしてツールバーに追加します。
3. 使用方法

　まず、対象の文書を開いて、取り出したいページに<緑色>の付箋を張り付けます。付箋の色は必ず<緑色>にしてください。他の色は無視されます。このようにして付箋で目印を付けた文書を選択し、プラグイン(T）メニューから「簡易ページ収集」を選びます。すると、付箋で印を付けたページだけを取り出した文書が作成されます。ファイルの名前は、元の名前の最後に「-抽出」を加えた物になります。
もし、<緑色>の付箋がどこにも無いならば、その文書には抽出結果の文書は作成されません。


4. ライセンス条項

　このソフトウェアは、富士ゼロックス株式会社の「DocuWorks(TM) Development Tool Kit 5.0.6」を用いて作成されています。

　このソフトウェアは、MITライセンスの元に提供されます。
--------------------------------------------------------------------------------------------------
MIT License

Copyright (c) 2003-2005,2017 sagamiya

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
--------------------------------------------------------------------------------------------------
ソースコードは、Microsoft Visual Stuido 2015用です。別途、「DocuWorks(TM) Development Tool Kit 8」を富士ゼロックスのWebサイトから入手する必要があります。

2017年3月5日 算譜渡世　相模屋

変更履歴
v1.2.2	2017.3.5	DocuWorks SDK ver.8 にてリコンパイル. MITライセンスに変更.
v1.2.1	2005.11.19	下記不具合が実際には修正されていなかった不具合を修正.
			不具合点を指摘いただいた SoftFarmの高瀬 様、ありがとうございます.
			DocuWorks6.0で使用できることを明記.
v1.2	2004.10.13	プラグイン名一覧にゴミが現れる不具合を修正.
v1.1	2004.10.10	dwsdk5.0.6 にて再コンパイル. 再配布ライブラリをdwsdk5.0.6の物に差替.
			ReadMe.txtの「概要」を訂正。
v1.0	2003.3.31 	初版リリース
