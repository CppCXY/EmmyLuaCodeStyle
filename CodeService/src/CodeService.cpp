#include <iostream>
#include "LuaParser/LuaParser.h"
#include "Util/format.h"

int main()
{
	auto parser = LuaParser::LoadFromBuffer(R"(
local t = {
aaa =123,
bbb,123.x,
dddd,
[123] =123,
["fyu"] =1,
[self] = "12313131"
} 
ffff(
aaa,
bbb)
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
