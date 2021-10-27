#include <iostream>
#include "LuaParser/LuaParser.h"
#include "Util/format.h"
#include "CodeService/LuaFormatter.h"
#include <fstream>

int main(int argc, char** argv)
{
	std::string source = R"(
if aaab+bbb +(ccc+ddd)
	+bbcs -fwfw  == 1 then

	end
	
	
	
		)";
	std::cout << "原文:\n" << source;
	auto parser = LuaParser::LoadFromBuffer(std::move(source));

	// if(argc < 2)
	// {
	// 	return -1;
	// }

	// auto parser = LuaParser::LoadFromBuffer(std::move(source));
	parser->BuildAstWithComment();

	// auto errors = parser->GetErrors();
	//
	// for (auto& err : errors)
	// {
	// 	std::cout << format("error: {} , textRange({},{})", err.ErrorMessage, err.ErrorRange.StartOffset,
	// 	                    err.ErrorRange.EndOffset) << std::endl;
	// }

	// auto comments = parser->GetAllComments();
	//
	// for (auto& comment : comments)
	// {
	// 	std::cout << format("comment is \n{}\n", comment.Text);
	// }

	auto ast = parser->GetAst();
	LuaFormatOptions options;
	LuaFormatter formatter(parser, options);
	formatter.BuildFormattedElement();
	std::cout << "\n格式化结果:\n";
	std::cout << formatter.GetFormattedText();

	// std::ofstream f(R"(C:\Users\zc\Desktop\learn\formatmobdebug.lua)", std::ios::out| std::ios::binary);

	// auto s = formatter.GetFormattedText();
	// f.write(s.c_str(), s.size());

	return 0;
}
