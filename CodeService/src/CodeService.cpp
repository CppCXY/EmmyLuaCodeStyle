#include <iostream>
#include "LuaParser/LuaTokenParser.h"
int main()
{
	std::string lua = R"(
	)";
	LuaTokenParser tokeParser(std::move(lua));
	tokeParser.Parse();

	return 0;
}