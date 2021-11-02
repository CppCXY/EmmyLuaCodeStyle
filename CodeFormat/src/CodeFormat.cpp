#include <iostream>
#include "LuaParser/LuaParser.h"
#include "Util/format.h"
#include "CodeService/LuaFormatter.h"
#include <fstream>

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		return -1;
	}

	std::shared_ptr<LuaParser> parser = nullptr;

	std::string cmdOption = argv[1];
	if(cmdOption == "-f")
	{
		parser = LuaParser::LoadFromFile(argv[1]);
	}
	else if(cmdOption == "-b")
	{
		std::size_t size = std::stoll(argv[2]);

		std::string buffer;
		buffer.resize(size);
		std::cin.get(buffer.data(), size, EOF);
		auto realSize = strnlen(buffer.data(), size);
		buffer.resize(realSize);
		parser = LuaParser::LoadFromBuffer(std::move(buffer));
	}

	parser->BuildAstWithComment();

	LuaFormatOptions options;
	options.LineSeperater = "\n";
	LuaFormatter formatter(parser, options);
	formatter.BuildFormattedElement();
	std::cout << formatter.GetFormattedText();
	return 0;
}