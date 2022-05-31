#include "LuaParser/LuaCustomParser.h"

#include "LuaDefine.h"
#include "LuaParser/LuaTokenTypeDetail.h"

int GetCurrentChar(std::string_view source, std::size_t i)
{
	if (i < source.size())
	{
		unsigned char ch = source[i];
		return ch;
	}
	return EOZ;
}

LuaTokenType LuaCustomParser::Lex(std::string_view source, std::size_t start, std::size_t& consumeSize)
{
	enum class ParseState
	{
		Unknown,
		Operator,
		Identify,
	} state = ParseState::Unknown;

	for (; (start + consumeSize) < source.size(); consumeSize++)
	{
		int ch = GetCurrentChar(source, start + consumeSize);
		if (ch == EOZ || ch == std::isspace(ch))
		{
			break;
		}
		switch (ch)
		{
		case '/':
		case '-':
		case '*':
		case '^':
		case '%':
		case '+':
		case '&':
		case '<':
		case '>':
		case '~':
		case '=':
			{
				if (state == ParseState::Unknown)
				{
					state = ParseState::Operator;
				}
				else if (state == ParseState::Identify)
				{
					goto endLoop;
				}

				break;
			}
		case '(':
		case ')':
		case '{':
		case '}':
		case '[':
		case ']':
			{
				goto endLoop;
			}
		default:
			{
				if (lislalpha(ch))
				{
					if (state == ParseState::Unknown)
					{
						state = ParseState::Identify;
					}
					else if (state == ParseState::Operator)
					{
						goto endLoop;
					}

					break;
				}
				goto endLoop;
			}
		}
	}
endLoop:

	if (consumeSize == 0)
	{
		return TK_UNKNOWN;
	}

	std::string_view tokenText = source.substr(start, consumeSize);
	auto it = _customTokens.find(tokenText);
	if (it == _customTokens.end())
	{
		return TK_UNKNOWN;
	}
	return it->second;
}

void LuaCustomParser::SetTokens(LuaTokenType type, const std::vector<std::string>& customTokens)
{
	for (auto it = _customTokens.begin(); it != _customTokens.end();)
	{
		if (it->second == type)
		{
			_customTokens.erase(it++);
			continue;
		}
		++it;
	}

	for (auto& token : customTokens)
	{
		_customTokens.insert({token, type});
	}
}

bool LuaCustomParser::IsSupportCustomTokens() const
{
	return !_customTokens.empty();
}
