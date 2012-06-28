node-voca
==============

はじめに
--------------
ICU を利用して、カタカナをローマ字および音声認識エンジン Julius の voca 形式に変換する Node.js のアドオンです。

インストール
--------------
git clone して次のコマンドを実行して下さい。

	$ node-waf configure build

wscript ファイル内の gcc のバージョンは適宜変更して下さい。デフォルトでは、g++-4.8 でコンパイルしています。

使い方
--------------
	var icu = require('./build/Release/icu');
	var str = 'キョーハイーテンキダナー';
	console.log(icu.kana2romaji(str));
	console.log(icu.kana2voca(str));

これで次のような出力となります。

	kyo:hai:tenkidana:
	ky o: h a i: t e N k i d a n a:

連携
--------------
node-mecab (https://github.com/hecomi/node-mecab) と組み合わせると、カタカナだけでない文字列をローマ字に出来ます。

	var MeCab = require('./node_addon/mecab/build/Release/mecab');
	var ICU   = require('./node_addon/icu/build/Release/icu');
	String.prototype.toRomaji = function() {
		return ICU.kana2romaji(MeCab.str2kana(this.toString()));
	}
	console.log('今日はいい天気だなぁ'.toRomaji());

詳細
--------------
その他詳細は Twitter: @hecomi までお問い合わせ下さい。


