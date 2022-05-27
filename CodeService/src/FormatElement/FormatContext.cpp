#include "CodeService/FormatElement/FormatContext.h"
#include "CodeService/FormatElement/TextElement.h"

FormatContext::FormatContext(std::shared_ptr<LuaParser> parser, LuaCodeStyleOptions& options)
	: _options(options),
	  _characterCount(0),
	  _parser(parser)
{
}

FormatContext::~FormatContext()
{
}

void FormatContext::AddIndent()
{
	if (_options.indent_style == IndentStyle::Space)
	{
		if (_indentStack.empty())
		{
			_indentStack.push_back({0, 0, IndentStyle::Space});
			return;
		}

		auto& topIndent = _indentStack.back();
		std::size_t newIndent = _options.indent_size + topIndent.SpaceIndent;

		_indentStack.push_back({newIndent, 0, IndentStyle::Space});
	}
	else
	{
		if (_indentStack.empty())
		{
			_indentStack.push_back({0, 0, IndentStyle::Tab});
			return;
		}

		auto& topIndent = _indentStack.back();
		IndentState state{topIndent.SpaceIndent, topIndent.TabIndent, IndentStyle::Tab};
		if (topIndent.SpaceIndent != 0)
		{
			state.SpaceIndent += _options.tab_width;
		}
		else
		{
			state.TabIndent += 1;
		}

		_indentStack.push_back(state);
	}
}

void FormatContext::AddIndent(IndentState state)
{
	_indentStack.push_back(state);
}

void FormatContext::RecoverIndent()
{
	if (_indentStack.empty())
	{
		return;
	}

	_indentStack.resize(_indentStack.size() - 1);
}

int FormatContext::GetLine(int offset)
{
	return _parser->GetLine(offset);
}

int FormatContext::GetColumn(int offset)
{
	return _parser->GetColumn(offset);
}

FormatContext::IndentState FormatContext::CalculateIndentState(int offset)
{
	auto file = _parser->GetLuaFile();
	auto source = file->GetSource();
	auto line = file->GetLine(offset);
	auto start = file->GetOffsetFromPosition(line, 0);


	IndentState state = {0, 0, IndentStyle::Space};
	for (; start < offset; start++)
	{
		auto ch = source[start];
		if (ch == '\t')
		{
			state.Style = IndentStyle::Tab;
			state.TabIndent++;
		}
		else
		{
			state.SpaceIndent++;
		}
	}

	if (state.TabIndent == 0 && state.SpaceIndent == 0)
	{
		state.Style = _options.indent_style;
	}

	return state;
}

std::size_t FormatContext::GetCharacterCount() const
{
	return _characterCount;
}

FormatContext::IndentState FormatContext::GetCurrentIndent() const
{
	if (_indentStack.empty())
	{
		return IndentState{0, 0, _options.indent_style};
	}

	return _indentStack.back();
}

FormatContext::IndentState FormatContext::GetLastIndent() const
{
	if (_indentStack.size() < 2)
	{
		return IndentState{0, 0, _options.indent_style};
	}

	return _indentStack[_indentStack.size() - 2];
}

bool FormatContext::OnlyEmptyCharBefore(int offset)
{
	auto file = _parser->GetLuaFile();
	return file->OnlyEmptyCharBefore(offset);
}

bool FormatContext::ShouldBreakLine(TextRange range)
{
	auto column = GetCharacterCount();
	if (column == 0)
	{
		return false;
	}

	auto startLine = GetLine(range.StartOffset);
	auto endLine = GetLine(range.EndOffset);

	if (startLine == endLine)
	{
		column += GetColumn(range.EndOffset) - GetColumn(range.StartOffset);
	}
	else
	{
		column += _parser->GetLuaFile()->GetLineRestCharacter(range.StartOffset);
	}

	return column >= _options.max_line_length;
}

std::shared_ptr<LuaParser> FormatContext::GetParser()
{
	return _parser;
}

LuaCodeStyleOptions& FormatContext::GetOptions()
{
	return _options;
}
