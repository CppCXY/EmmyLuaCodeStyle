#include "CodeService/TypeFormat/LuaTypeFormat.h"
#include <algorithm>
#include "LuaParser/LuaTokenTypeDetail.h"
#include "CodeService/FormatElement/SerializeContext.h"
#include "Util/StringUtil.h"

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

int FindTokenIndexAfterPosition(std::vector<LuaToken>& tokens, int offset)
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
		return -1;
	}

	return static_cast<int>(pos - tokens.begin());
}

std::shared_ptr<LuaAstNode> FindAstNodeBeforePosition(std::shared_ptr<LuaAstNode> root, int offset)
{
	if (root->GetChildren().empty())
	{
		return root;
	}

	enum class FindState
	{
		None,
		Before,
		Contain
	} state = FindState::None;

	auto it = root->GetChildren().begin();
	for (; it != root->GetChildren().end(); it++)
	{
		auto& child = *it;
		auto textRange = child->GetTextRange();
		if (textRange.StartOffset > offset)
		{
			state = FindState::Before;
			break;
		}
		else if (offset <= textRange.EndOffset)
		{
			state = FindState::Contain;
			break;
		}
	}

	switch (state)
	{
	case FindState::Before:
		{
			if (it == root->GetChildren().begin())
			{
				return root;
			}
			return *(it - 1);
		}
	case FindState::Contain:
		{
			return FindAstNodeBeforePosition(*it, offset);
		}
	case FindState::None:
		{
			return root->GetChildren().back();
		}
	}
	return nullptr;
}


LuaTypeFormat::LuaTypeFormat(std::shared_ptr<LuaParser> luaParser, LuaCodeStyleOptions& options,
                             LuaTypeFormatOptions& typeOptions)
	: _parser(luaParser),
	  _options(options),
	  _typeOptions(typeOptions),
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

std::vector<LuaTypeFormat::Result>& LuaTypeFormat::GetResult()
{
	return _results;
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

	if (_typeOptions.format_line)
	{
		FormatLine(line, character);
	}

	// if (_typeOptions.fix_indent) {
	// 	FixIndent(line, character);
	// }
}

void LuaTypeFormat::CompleteMissToken(int line, int character, LuaError& luaError)
{
	if (!_typeOptions.auto_complete_end)
	{
		return;
	}
	auto& result = _results.emplace_back();

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
				result.Text = ctx.GetText();
				result.Range.StartLine = line;
				result.Range.StartCharacter = character;
				result.Range.EndLine = line;
				result.Range.EndCharacter = character;
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
				result.Text = ctx.GetText();
				result.Range.StartLine = line + 1;
				result.Range.StartCharacter = 0;
				result.Range.EndLine = line + 1;
				result.Range.EndCharacter = 0;
			}
			break;
		}
	default:
		{
			break;
		}
	}
}

void LuaTypeFormat::FormatLine(int line, int character)
{
	auto luaFile = _parser->GetLuaFile();
	auto offset = luaFile->GetOffsetFromPosition(line, character);
	auto& tokens = _parser->GetTokenParser()->GetTokens();
	auto tokenIndex = FindTokenIndexAfterPosition(tokens, offset);
	if (tokenIndex != -1)
	{
		auto& token = tokens[tokenIndex];
		if (token.TokenType == TK_STRING || token.TokenType == TK_LONG_COMMENT)
		{
			return;
		}
	}

	LuaCodeStyleOptions temp = _options;
	temp.insert_final_newline = true;
	temp.remove_expression_list_finish_comma = false;
	if (_typeOptions.auto_complete_table_sep)
	{
		temp.trailing_table_separator = TrailingTableSeparator::Smart;
	}
	else
	{
		temp.trailing_table_separator = TrailingTableSeparator::Keep;
	}

	LuaFormatter formatter(_parser, temp);
	formatter.BuildFormattedElement();

	auto& result = _results.emplace_back();
	result.Range.StartLine = line - 1;
	result.Range.StartCharacter = 0;
	result.Range.EndLine = line - 1;
	result.Range.EndCharacter = 0;
	auto formatText = formatter.GetRangeFormattedText(result.Range);
	while (!formatText.empty())
	{
		char ch = formatText.back();
		if (ch == ' ')
		{
			formatText.pop_back();
		}
		else
		{
			break;
		}
	}
	if (!formatText.empty() && formatText.back() != '\n')
	{
		formatText.push_back('\n');
	}
	result.Text = formatText;
	result.Range.EndLine++;
	_hasResult = true;
}

void LuaTypeFormat::FixIndent(int line, int character)
{
	FixEndIndent(line, character);

	// auto root = _parser->GetAst();
	// auto astNode = FindAstNodeBeforePosition(root, offset);
	// if (!astNode)
	// {
	// 	return;
	// }

	// auto block = astNode->GetParent();
	// if (block && block->GetType() == LuaAstNodeType::Block)
	// {
	// 	auto statement = block->GetParent();
	// 	if(statement->is)
	//
	//
	// }


}

void LuaTypeFormat::FixEndIndent(int line, int character)
{
	auto luaFile = _parser->GetLuaFile();
	auto offset = luaFile->GetOffsetFromPosition(line, character);

	auto& tokens = _parser->GetTokenParser()->GetTokens();
	auto tokenIndex = FindTokenIndexAfterPosition(tokens, offset);
	if (tokenIndex == -1)
	{
		return;
	}

	auto token = tokens[tokenIndex];
	if (token.TokenType == TK_END && luaFile->GetLine(token.Range.StartOffset) == line)
	{
		_hasResult = true;
		auto indentString = luaFile->GetIndentString(token.Range.StartOffset);
		{
			auto& result = _results.emplace_back();
			SerializeContext ctx1(_parser, _options);
			ctx1.Print(indentString);
			ctx1.PrintIndent(4, _options.indent_style);
			result.Text = ctx1.GetText();
			result.Range.StartLine = line;
			result.Range.StartCharacter = character;
			result.Range.EndLine = line;
			result.Range.EndCharacter = character;
		}
		{
			SerializeContext ctx2(_parser, _options);
			auto& result = _results.emplace_back();
			ctx2.PrintLine(1);
			ctx2.Print(indentString);
			ctx2.Print("e");
			result.Text = ctx2.GetText();
			result.Range.StartLine = luaFile->GetLine(token.Range.StartOffset);
			result.Range.StartCharacter = luaFile->GetColumn(token.Range.StartOffset);
			result.Range.EndLine = result.Range.StartLine;
			result.Range.EndCharacter = result.Range.StartCharacter + 1;
		}
	}
}
