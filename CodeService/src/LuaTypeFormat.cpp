#include "CodeService/LuaTypeFormat.h"
#include <algorithm>
#include "LuaParser/LuaTokenTypeDetail.h"
#include "CodeService/FormatElement/SerializeContext.h"

int FindTokenIndexBeforePosition(std::vector<LuaToken>& tokens, int offset)
{
	if (tokens.empty())
	{
		return -1;
	}

	auto tempToken = LuaToken(0, "", TextRange(offset, offset));
	// 二分法的思想快速定位
	auto pos = std::lower_bound(
		tokens.begin(), tokens.end(), tempToken,
		[](const LuaToken& x, const LuaToken& y)
		{
			return x.Range.EndOffset < y.Range.StartOffset;
		});
	if (pos == tokens.end())
	{
		return static_cast<int>(tokens.size()) - 1;
	}

	return static_cast<int>(pos - tokens.begin()) - 1;
}


LuaTypeFormat::LuaTypeFormat(std::shared_ptr<LuaParser> luaParser, LuaCodeStyleOptions& options)
	: _parser(luaParser),
	  _options(options),
	  _hasResult(false)
{
}

void LuaTypeFormat::Analysis(std::string_view trigger, int line, int character)
{
	if (trigger == "\n")
	{
		return AnalysisReturn(line, character);
	}
}

bool LuaTypeFormat::HasFormatResult()
{
	return _hasResult;
}

LuaTypeFormat::Result& LuaTypeFormat::GetResult()
{
	return _result;
}

void LuaTypeFormat::AnalysisReturn(int line, int character)
{
	if (!_parser)
	{
		return;
	}

	auto luaFile = _parser->GetLuaFile();

	if (_parser->HasError())
	{
		auto& errors = _parser->GetErrors();
		if (!errors.empty())
		{
			if (errors.front().MissToken != 0)
			{
				return CompleteMissToken(line, character, errors.front());
			}
		}
		return;
	}

	if (luaFile->IsEmptyLine(line - 1))
	{
		return;
	}

	FormatLine(line);
}

void LuaTypeFormat::CompleteMissToken(int line, int character, LuaError& luaError)
{
	LuaCodeStyleOptions temp = _options;
	temp.insert_final_newline = true;
	switch (luaError.MissToken)
	{
	case TK_END:
		{
			auto tokenParser = _parser->GetTokenParser();
			auto luaFile = tokenParser->GetFile();
			auto& tokens = tokenParser->GetTokens();
			auto offset = luaFile->GetOffsetFromPosition(line, character);
			const int tokenIndex = FindTokenIndexBeforePosition(tokens, offset);
			if (tokenIndex == -1)
			{
				return;
			}


			auto& token = tokens[static_cast<std::size_t>(tokenIndex)];
			auto keyOffset = token.Range.StartOffset;
			switch (token.TokenType)
			{
			case TK_DO:
			case TK_THEN:
			case TK_ELSE:
				{
					break;
				}
			case ')':
				{
					for (int i = tokenIndex - 1; i > 0; i --)
					{
						auto& testToken = tokens[i];
						if (testToken.TokenType == TK_FUNCTION)
						{
							keyOffset = testToken.Range.StartOffset;
							break;
						}
					}
					break;
				}
			default:
				{
					return;
				}
			}

			bool nextBrace = false;
			auto indentString = luaFile->GetIndentString(keyOffset);
			if (static_cast<std::size_t>(tokenIndex) + 1 != tokens.size())
			{
				auto& nextToken = tokens[tokenIndex + 1];
				if (nextToken.TokenType == TK_END)
				{
					// 依据缩进判断是否该填补end
					auto endIndentString = luaFile->GetIndentString(nextToken.Range.StartOffset);
					if (indentString == endIndentString)
					{
						return;
					}
				}
				else if (nextToken.TokenType == ')')
				{
					nextBrace = true;
				}
			}

			_hasResult = true;
			SerializeContext ctx(_parser, temp);
			if (nextBrace)
			{
				ctx.PrintLine(1);
				ctx.Print(indentString);
				ctx.Print("end");
				_result.Text = ctx.GetText();
				_result.Range.StartLine = line;
				_result.Range.StartCharacter = character;
				_result.Range.EndLine = line;
				_result.Range.EndCharacter = character;
			}
			else
			{
				auto totalLine = _parser->GetLuaFile()->GetTotalLine();
				if (line >= totalLine)
				{
					ctx.PrintLine(1);
				}
				ctx.Print(indentString);
				ctx.Print("end");
				ctx.PrintLine(1);
				_result.Text = ctx.GetText();
				_result.Range.StartLine = line + 1;
				_result.Range.StartCharacter = 0;
				_result.Range.EndLine = line + 1;
				_result.Range.EndCharacter = 0;
			}
			break;
		}
	default:
		{
			break;
		}
	}
}

void LuaTypeFormat::FormatLine(int line)
{
	LuaCodeStyleOptions temp = _options;
	temp.insert_final_newline = true;
	LuaFormatter formatter(_parser, temp);
	formatter.BuildFormattedElement();

	_result.Range.StartLine = line - 1;
	_result.Range.StartCharacter = 0;
	_result.Range.EndLine = line - 1;
	_result.Range.EndCharacter = 0;
	_result.Text = formatter.GetRangeFormattedText(_result.Range);
	_result.Range.EndLine++;
	_hasResult = true;
}
