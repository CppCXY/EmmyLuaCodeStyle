//#pragma once
//
//#include <string_view>
//#include <map>
//#include "LuaToken.h"
//
//class LuaDocTokenParser
//{
//public:
//	LuaDocTokenParser(std::string_view doc, TextRange range);
//	~LuaDocTokenParser();
//
//	LuaToken& Current();
//
//	void Next();
//
//private:
//
//	static std::map<std::string, LuaTokenType, std::less<>> LuaDocReserved;
//
//	LuaTokenType Lex();
//
//	LuaTokenType TagLex();
//
//	int NextChar();
//
//	void SaveAndNext();
//
//	int GetCurrentChar();
//
//	void ResetBuffer();
//
//	std::string_view GetSaveText();
//
//	std::string_view _source;
//	TextRange _range;
//	LuaToken _current;
//
//	bool _hasSaveText;
//	std::size_t _buffStart;
//	std::size_t _buffIndex;
//
//	bool _hasEoz;
//	std::size_t _currentParseIndex;
//
//	bool _docTagFounded;
//};
