#pragma once

#include <map>
#include <string_view>
#include <memory>
#include <string>
#include <vector>
#include <set>

#include "LuaToken.h"
#include "LuaTokenKind.h"
#include "LuaParser/File/LuaFile.h"
#include "TextReader.h"
#include "LuaTokenError.h"

/*
 * token 解析来自于lua 源代码,实现上非常接近但细节处并不相同
 */
class LuaLexer
{
public:
	explicit LuaLexer(std::shared_ptr<LuaFile> file);

    bool Parse();

	std::vector<LuaTokenError>& GetErrors();

	bool HasError() const;

	std::shared_ptr<LuaFile> GetFile();

	std::vector<LuaToken>& GetTokens();

    void SupportNonStandardSymbol();

//	void SetCustomParser(std::shared_ptr<LuaCustomParser> parser);
private:
	static std::map<std::string, LuaTokenKind, std::less<>> LuaReserved;

	LuaTokenKind Lex();

	LuaTokenKind ReadNumeral();

	std::size_t SkipSep();

	void ReadLongString(std::size_t sep);

	void ReadString(int del);

	void IncLinenumber();

	bool CurrentIsNewLine();

	bool IsReserved(std::string_view text);

	void TokenError(std::string_view message, TextRange range);

	int _linenumber;
    bool _supportNonStandardSymbol;
    TextReader _reader;
	std::vector<LuaToken> _tokens;
	std::vector<LuaTokenError> _errors;
	std::shared_ptr<LuaFile> _file;
};
