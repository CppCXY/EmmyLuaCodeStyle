#include <iostream>
#include "LuaParser/LuaParser.h"
#include "Util/format.h"
#include "CodeService/LuaFormatter.h"
#include <fstream>

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		return -1;
	}

	auto parser = LuaParser::LoadFromFile(argv[1]);
	parser->BuildAstWithComment();

	// auto errors = parser->GetErrors();
	//
	// for (auto& err : errors)
	// {
	// 	std::cout << format("error: {} , textRange({},{})", err.ErrorMessage, err.ErrorRange.StartOffset,
	// 	                    err.ErrorRange.EndOffset) << std::endl;
	// }
	auto ast = parser->GetAst();
	LuaFormatOptions options;
	LuaFormatter formatter(parser, options);
	formatter.BuildFormattedElement();
	std::cout << formatter.GetFormattedText();
	return 0;
}
