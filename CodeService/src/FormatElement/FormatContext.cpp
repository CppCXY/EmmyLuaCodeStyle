#include "CodeService/FormatElement/FormatContext.h"

FormatContext::FormatContext(std::shared_ptr<LuaParser> parser, LuaFormatOptions& options)
	: _options(options),
	  _characterCount(0),
	  _parser(parser)
{

}

void FormatContext::Print(std::string_view text)
{
	auto& indentPair = _indentStack.top();
	if (_characterCount < indentPair.Indent)
	{
		PrintBlank(indentPair.Indent - _characterCount);
	}
	_os << text;
	_characterCount += text.size();
}

void FormatContext::PrintLine(int line)
{
	for (int i = 0; i != line; i++)
	{
		_os << _options.LineSeperater;
		_characterCount = 0;
	}
}

void FormatContext::PrintBlank(int blank)
{
	for (int i = 0; i != blank; i ++)
	{
		_os << " ";
		_characterCount++;
	}
}

void FormatContext::AddIndent()
{
	if(_indentStack.empty())
	{
		_indentStack.push({ 0, "" });
		return ;
	}

	auto& topIndent = _indentStack.top();
	int newIndent = _options.Indent + topIndent.Indent;

	std::string indentString;
	indentString.reserve(_options.IndentString.size() * newIndent);
	for (int i = 0; i != newIndent; i++)
	{
		indentString.append(_options.IndentString);
	}
	_indentStack.push({newIndent, std::move(indentString)});
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

std::string FormatContext::GetText()
{
	return _os.str();
}
