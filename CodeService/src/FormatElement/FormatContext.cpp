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
			_indentStack.push_back({0, IndentStyle::Space});
			return;
		}

		auto& topIndent = _indentStack.back();
		std::size_t newIndent = _options.indent_size + topIndent.Indent;

		_indentStack.push_back({newIndent, IndentStyle::Space});
	}
	else
	{
		if (_indentStack.empty())
		{
			_indentStack.push_back({0, IndentStyle::Tab});
			return;
		}

		auto& topIndent = _indentStack.back();

		_indentStack.push_back({1 + topIndent.Indent, IndentStyle::Tab});
	}
}

void FormatContext::AddIndent(std::size_t specialIndent, IndentStyle style)
{
	_indentStack.push_back({specialIndent, style});
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

std::size_t FormatContext::GetCharacterCount() const
{
	return _characterCount;
}

std::size_t FormatContext::GetCurrentIndent() const
{
	if (_indentStack.empty())
	{
		return 0;
	}

	return _indentStack.back().Indent;
}

std::size_t FormatContext::GetLastIndent() const
{
	if (_indentStack.size() < 2)
	{
		return 0;
	}

	return _indentStack[_indentStack.size() - 2].Indent;
}

std::shared_ptr<LuaParser> FormatContext::GetParser()
{
	return _parser;
}

LuaCodeStyleOptions& FormatContext::GetOptions()
{
	return _options;
}
