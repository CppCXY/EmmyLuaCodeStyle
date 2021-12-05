#pragma once

#include <map>
#include <string_view>
#include <string>
#include <vector>
#include <set>

#include "LuaToken.h"
#include "LuaTokenType.h"

/*
 * token 解析来自于lua 源代码,实现上非常接近但细节处并不相同
 */
class LuaTokenParser
{
public:
	LuaTokenParser(std::string&& source);

	bool Parse();

	LuaToken& Next();

	LuaToken& LookAhead();

	LuaToken& Current();

	int LastValidOffset();

	int GetLine(int offset);
	
	int GetColumn(int offset);

	int GetTotalLine();

	bool IsEmptyLine(int line);

	std::string& GetSource();

	std::vector<LuaToken>& GetComments();

	std::vector<LuaError>& GetErrors();

	bool HasError() const;
private:
	static std::map<std::string, LuaTokenType, std::less<>> LuaReserved;

	LuaTokenType Lex();

	int NextChar();

	void SaveAndNext();

	void Save();

	int GetCurrentChar();

	bool CheckNext1(int ch);

	bool CheckNext2(std::string_view set);

	LuaTokenType ReadNumeral();

	std::size_t SkipSep();

	void ReadLongString(std::size_t sep);

	void ReadString(int del);

	void IncLinenumber();

	bool CurrentIsNewLine();

	void ResetBuffer();

	std::string_view GetSaveText() const;

	bool IsReserved(std::string_view text);

	void PushLuaError(std::string_view message, TextRange range);

	int _linenumber;

	bool _hasSaveText;
	std::size_t _buffStart;
	std::size_t _buffIndex;

	bool _hasEoz;
	std::size_t _currentParseIndex;
	std::size_t _currentIndex;
	std::string _source;

	std::vector<LuaToken> _tokens;
	// 注释单独处理
	std::vector<LuaToken> _commentTokens;

	std::vector<::LuaError> _errors;

	LuaToken _eosToken;

	std::vector<int> _lineOffsetVec;
};
