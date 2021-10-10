#include <iostream>
#include "LuaParser/LuaParser.h"

int main()
{
	std::cout << "hello world" << std::endl;
	auto parser = LuaParser::LoadFromFile(R"xxx");
	auto ast = parser->GetAst();
	for(auto astNode: ast->GetChildren())
	{
		for (auto a : astNode->GetChildren()) {
			std::cout << a->GetText();
		}
	}
	
	return 0;
}