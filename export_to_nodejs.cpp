#include <unicode/translit.h>
#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
#include <map>
#include <node.h>

using namespace v8;

/**
 * ICU でカタカナをローマ字に変換する
 * @param[in] args JavaScript側から渡される引数
 * @return 正常な場合は ICU で変換した結果のローマ字、ダメな場合は undefined
 */
Handle<Value> kana2romaji(const Arguments& args)
{
	HandleScope scope;

	// 引数が文字列かどうかチェック
	if (!args[0]->IsString()) {
		Local<String> msg = String::New("[MeCab] Error! Argument of 'parse' must be String.");
		ThrowException(Exception::TypeError(msg));
		return scope.Close(Undefined());
	}

	String::Utf8Value str(args[0]);
	UnicodeString input = *str;

	// カタカナ --> Latin 変換
	UErrorCode error = U_ZERO_ERROR;
	boost::shared_ptr<Transliterator> t(
		Transliterator::createInstance("Katakana-Latin", UTRANS_FORWARD, error)
	);
	t->transliterate(input);

	// 伸ばす音の表記変更
	std::map<UnicodeString, UnicodeString> long_map = {
		{"\u0101","a:"},
		{"\u0113","e:"},
		{"\u012B","i:"},
		{"\u014D","o:"},
		{"\u016B","u:"},
	};
	for (const auto& x : long_map) {
		input.findAndReplace(x.first, x.second);
	}

	// 変換結果取得
	size_t length = input.length();
	char* result = new char[length + 1];
	input.extract(0, length, result, "utf8");

	return scope.Close(String::New(result));
}

/**
 * ICU でカタカナをローマ字に変換してから音声認識エンジン Julius の voca ファイル形式に変換する
 * (e.g. イースタートー --> i: s u t a: t o:)
 * @param[in] args JavaScript側から渡される引数
 * @return 正常な場合は Julius の発音形式の文字列、ダメな場合は undefined
 */
Handle<Value> kana2voca(const Arguments& args)
{
	HandleScope scope;

	// 引数が文字列かどうかチェック
	if (!args[0]->IsString()) {
		Local<String> msg = String::New("[MeCab] Error! Argument of 'parse' must be String.");
		ThrowException(Exception::TypeError(msg));
		return scope.Close(Undefined());
	}

	String::Utf8Value str(args[0]);
	UnicodeString input = *str;

	// 「ン」をマーキング
	input.findAndReplace("ン", "[[ン]]");

	// カタカナ --> Latin 変換
	UErrorCode error = U_ZERO_ERROR;
	boost::shared_ptr<Transliterator> t(
		Transliterator::createInstance("Katakana-Latin", UTRANS_FORWARD, error)
	);
	t->transliterate(input);

	// 伸ばす音の表記変更 + マーキングしたンをNにする + 「つ」を「q」にする
	std::map<UnicodeString, UnicodeString> long_map = {
		{"\u0101","a:"},
		{"\u0113","e:"},
		{"\u012B","i:"},
		{"\u014D","o:"},
		{"\u016B","u:"},
		{"[[n]]" , "N"},
		{"~"     , "q"}
	};
	for (const auto& x : long_map) {
		input.findAndReplace(x.first, x.second);
	}

	// 変換結果取得
	size_t length = input.length();
	char* romaji  = new char[length + 1];
	input.extract(0, length, romaji, "utf8");

	// Julius の voca 形式へ整形
	std::string result(romaji);
	std::map<std::string, std::string> regex_map = {
		{"[aiueoNq]:?"     , "$0 "},
		{"[^aiueoNq]{1,2}" , "$0 "},
		{"[^a-zN:@]"       , ""   },
		{"\\s+"            , " "  },
	};
	for (const auto& x : regex_map) {
		boost::regex r(x.first);
		result = boost::regex_replace(result, r, x.second, boost::format_all);
	}

	return scope.Close(String::New(result.c_str()));
}

/**
 * Node.js の世界へいってらっしゃい
 */
void init(Handle<Object> target) {
	HandleScope scope;
	target->Set(
		String::NewSymbol("kana2romaji"),
		FunctionTemplate::New(kana2romaji)->GetFunction()
	);
	target->Set(
		String::NewSymbol("kana2voca"),
		FunctionTemplate::New(kana2voca)->GetFunction()
	);
}

NODE_MODULE(icu, init)

