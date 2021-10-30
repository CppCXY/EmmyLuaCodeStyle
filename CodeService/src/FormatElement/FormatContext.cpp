#include "CodeService/FormatElement/FormatContext.h"

FormatContext::FormatContext(std::shared_ptr<LuaParser> parser, LuaFormatOptions& options)
	: _options(options),
	  _characterCount(0),
	  _parser(parser),
	  _linebreakIndent(0)
{
}

void FormatContext::Print(std::string_view text)
{
	auto& indentState = _indentStack.top();
	if (_characterCount < indentState.Indent)
	{
		PrintBlank(indentState.Indent - static_cast<int>(_characterCount));
	}
	_os << text;
	_characterCount += text.size();
}

void FormatContext::PrintLine(int line)
{
	for (int i = 0; i < line; i++)
	{
		_os << _options.LineSeperater;
		_characterCount = 0;
	}

	if(_indentStack.top().Type == IndentStateType::ActiveIfLineBreak)
	{
		// ¸´ÖÆ
		auto topIndent = _indentStack.top();
		_indentStack.pop();

		AddIndent(topIndent.WaitActiveIndent);
	}
}

void FormatContext::PrintBlank(int blank)
{
	for (int i = 0; i < blank; i ++)
	{
		_os << " ";
		_characterCount++;
	}
}

void FormatContext::AddIndent(int specialIndent, IndentStateType type)
{
	if (type == IndentStateType::Normal)
	{
		int newIndent = 0;
		if (specialIndent == -1)
		{
			if (_indentStack.empty())
			{
				_indentStack.push({0, 0, "", type});
				return;
			}

			auto& topIndent = _indentStack.top();
			newIndent = _options.Indent + topIndent.Indent;
		}
		else
		{
			newIndent = specialIndent;
		}
		std::string indentString;
		indentString.reserve(_options.IndentString.size() * newIndent);
		for (int i = 0; i != newIndent; i++)
		{
			indentString.append(_options.IndentString);
		}
		_indentStack.push({newIndent, 0, std::move(indentString), type});
	}
	else if(type == IndentStateType::ActiveIfLineBreak)
	{
		// ¸´ÖÆ
		auto topIndent = _indentStack.top();
		topIndent.WaitActiveIndent = specialIndent;
		topIndent.Type = type;
		_indentStack.push(topIndent);
	}
}

void FormatContext::RecoverIndent()
{
	_indentStack.pop();
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

	return _indentStack.top().Indent;
}

std::string FormatContext::GetText()
{
	return _os.str();
}
