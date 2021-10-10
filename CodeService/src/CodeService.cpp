#include <iostream>
#include "LuaParser/LuaParser.h"

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
)");
	auto ast = parser->GetAst();
	for (auto astNode : ast->GetChildren())
	{
		for (auto a : astNode->GetChildren())
		{
			std::cout << a->GetText() << std::endl;
		}
	}

	return 0;
}
