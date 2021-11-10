#include <iostream>
#include <cstring>
#include <fstream>
#include "LuaParser/LuaParser.h"
#include "Util/format.h"
#include "CodeService/LuaFormatter.h"


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
		parser = LuaParser::LoadFromFile(argv[2]);
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
	LuaFormatOptions options;
	if(argc >= 5)
	{
		std::string configOption = argv[3];
		if(configOption == "-c")
		{
			options = LuaFormatOptions::ParseFromEditorConfig(argv[4]);
		}
	}

	parser->BuildAstWithComment();

	LuaFormatter formatter(parser, options);
	formatter.BuildFormattedElement();
	auto formattedText = formatter.GetFormattedText();
	std::cout.write(formattedText.data(), formattedText.size());
	return 0;
}
