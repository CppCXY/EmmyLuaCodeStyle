#pragma once

#include <map>
#include <string_view>
#include <memory>
#include <string>
#include <vector>
#include <set>

#include "LuaCompile/Lua/Define/LuaSyntaxError.h"
#include "LuaCompile/Lua/Define/LuaToken.h"
#include "LuaCompile/Lua/Kind/LuaTokenKind.h"
#include "LuaCompile/Lua/Util/TextReader.h"

/*
 * token 解析来自于lua 源代码,实现上非常接近但细节处并不相同
 */
class LuaLexer
{
public:
	explicit LuaLexer(std::string_view text);

    std::vector<LuaToken>& Tokenize();

    std::vector<LuaSyntaxError>& GetErrors();

    static bool IsReserved(std::string_view text);
private:
	static std::map<std::string, LuaTokenKind, std::less<>> LuaReserved;

    static bool IsWhitespace(int ch);

	LuaTokenKind Lex();

	LuaTokenKind ReadNumeral();

	std::size_t SkipSep();

	void ReadLongString(std::size_t sep);

	void ReadString(int del);

	bool CurrentIsNewLine();

    void ReadNewLine();

	void TokenError(std::string_view message, TextRange range);

    void TokenError(std::string_view message, std::size_t offset);

    TextReader _reader;
	std::vector<LuaToken> _tokens;
    std::vector<LuaSyntaxError> _errors;
};
