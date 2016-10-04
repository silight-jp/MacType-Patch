このソフトウェアはMacTypeのパッチです。

使い方
1. MacTypeに付属していたEasyHK32.dllとEasyHK64.dll（64bit版のみ）は削除かリネームする。
2. UserParams.iniをMacTypeのフォルダに入れる。
3. DLLを「MacTypeのフォルダ」と「システムフォルダ（下記の通り）」の「両方」にコピーする。
32bit
	C:\Windows\System32\EasyHK32.dll
64bit
	C:\Windows\System32\EasyHK64.dll
	C:\Windows\SysWOW64\EasyHK32.dll

後はUserParams.iniを適当に改造してください。

更新履歴
2016/08/28	1.13	Direct2Dの新バージョンに対応
			印刷でレイアウトが崩れる可能性があったのを修正
			DirectWriteを使わないソフトではDirectWriteを初期化しないように修正
2016/07/16	1.12	開発環境をアップデートした
			EasyHookをバージョンアップした
			少し高速化
2016/06/27	1.11	ReplaceDirectWriteWithGDIを廃止
			少し高速化
2016/06/22	1.10	パラメーターを減らして必要最小限のフックしかしないようにした
2016/06/20	1.9	IsPixelSnappingDisabledを廃止
2016/06/19	1.8	ストアアプリにも効くようにした
2016/06/18	1.7	MacTypeの文字が少し欠けるバグを修正
			コンパイルオプションをMacTypeになるべく合わせてみた
2016/06/14	1.6	安全性が増したかもしれない
2016/06/11	1.5	UserParams.iniの仕様を大幅変更
			Firefoxで安定性が増した
			DirectWriteをGDI(MacType)に置き換える機能を試験的に実装
			ReplaceDirectWriteWithGDI=1で使える
2016/06/09	1.4	1.3で稀にデッドロックが発生する問題を修正
			ガンマ値の計算が間違っていたのを修正
2016/06/08	1.3	いくつかのアプリで起動時にクラッシュするバグを修正
			FontSubstitutesが効かなくなっていたのを修正
			MacTypeのUnicodeの文字が半分欠ける問題を修正
2016/06/05	1.2	D2D1AntialiasModeを廃止（ALIASEDでもアンチエイリアスはかかる）
			フックを安全な方式に変更
			MacTypeの縦書きバグを部分的に修正
2016/06/04	1.1	UserParams.iniを更新
			DWriteTextAntialiasModeを廃止
			HookTargetを追加
2016/06/01	1.0	公開
