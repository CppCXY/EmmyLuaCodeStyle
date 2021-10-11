#include <iostream>
#include "LuaParser/LuaParser.h"
#include "Util/format.h"

int main()
{
	auto parser = LuaParser::LoadFromBuffer(R"(
ccc

)");
	// auto ast = parser->GetAst();
	auto errors = parser->GetErrors();

	for (auto& err : errors)
	{
		std::cout << format("error: {} , textRange({},{})", err.ErrorMessage, err.ErrorRange.StartOffset,
		                    err.ErrorRange.EndOffset) << std::endl;
	}

	return 0;
}
