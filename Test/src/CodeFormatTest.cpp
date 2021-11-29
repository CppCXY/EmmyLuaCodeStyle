#include <cassert>
#include <iostream>
#include "CodeService/LuaFormatter.h"


std::string_view TrimSpace(std::string_view source)
{
	if (source.empty())
	{
		return source;
	}

	std::size_t start = 0;
	for (; start != source.size(); start++)
	{
		char ch = source[start];
		if (ch != '\n' && ch != '\r' && ch != ' ')
		{
			break;
		}
	}

	int end = source.size() - 1;
	for (; end >= 0; end--)
	{
		char ch = source[end];
		if (ch != '\n' && ch != '\r' && ch != ' ')
		{
			break;
		}
	}

	if (end < static_cast<int>(start))
	{
		return "";
	}
	return source.substr(start, end - start + 1);
}

void Test(std::string input, const std::string& shouldBe, std::shared_ptr<LuaCodeStyleOptions> options)
{
	auto parser = LuaParser::LoadFromBuffer(std::move(input));
	parser->BuildAstWithComment();

	LuaFormatter formatter(parser, *options);

	formatter.BuildFormattedElement();

	auto formattedText = formatter.GetFormattedText();

	if (TrimSpace(formattedText) != TrimSpace(shouldBe))
	{
		throw std::exception();
	}
}

int main()
{
	auto options = std::make_shared<LuaCodeStyleOptions>();
	options->line_separator = '\n';
	try
	{
		Test(
			"local t=123 --fff",
			"local t = 123 --fff",
			options
		);

		Test(
			R"(
local t  = 123
local cc = 123
)",
			R"(
local t  = 123
local cc = 123
)",
			options
		);

		Test(
			R"(
function fff()
local t =123
end
)",
R"(
function fff()
    local t = 123
end
)",
options
);

		Test(
			R"(
do return end
)",
R"(
do return end
)",
options
);

		Test(
			R"(
local f = function (x,y) return x,y end
)",
R"(
local f = function(x, y) return x, y end
)",
options
);
	}
	catch (std::exception& e)
	{
		return -1;
	}

	return 0;
}
