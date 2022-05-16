#pragma once

#include <map>
#include <string_view>
#include <memory>
#include <string>
#include <vector>
#include <set>

#include "LuaToken.h"
#include "LuaTokenType.h"
#include "LuaFile.h"

/*
 * token 解析来自于lua 源代码,实现上非常接近但细节处并不相同
 */
class LuaTokenParser
{
public:
	LuaTokenParser(std::shared_ptr<LuaFile> file);

	bool Parse();

	LuaToken& Next();

	LuaToken& LookAhead();

	LuaToken& Current();

	int LastValidOffset();

	std::string_view GetSource();

	std::vector<LuaToken>& GetComments();

	std::vector<LuaError>& GetErrors();

	bool HasError() const;

	void ReleaseTokens();

	std::shared_ptr<LuaFile> GetFile();

	bool ConsumeAllTokens();

	std::vector<LuaToken>& GetTokens();
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
	std::string_view _source;

	std::vector<LuaToken> _tokens;
	// 注释单独处理
	std::vector<LuaToken> _commentTokens;

	std::vector<LuaError> _errors;

	LuaToken _eosToken;

	std::shared_ptr<LuaFile> _file;
};
