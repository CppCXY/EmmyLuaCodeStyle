#include <iostream>
#include "LuaParser/LuaParser.h"
#include "Util/format.h"
#include "CodeService/LuaFormatter.h"

int main()
{
	std::string source = R"(
-- Cache some library functions and objects.
local jit = require("jit")
assert(jit.version_num == 20100, "LuaJIT core/library version mismatch")
local profile = require("jit.profile")
local vmdef = require("jit.vmdef")
local math = math
local pairs, ipairs, tonumber, floor = pairs, ipairs, tonumber, math.floor
local sort, format = table.sort, string.format
local stdout = io.stdout
local zone  -- Load jit.zone module on demand.

-- Output file handle.
local out 

------------------------------------------------------------------------------

local prof_ud 
local prof_states, prof_split, prof_min, prof_raw, prof_fmt, prof_depth 
local prof_ann, prof_count1, prof_count2, prof_samples 

local map_vmmode = {
    N = "Compiled",
    I = "Interpreted",
    C = "C code",
    G = "Garbage Collector",
    J = "JIT Compiler",
}

)";
	std::cout << "原文:\n" << source;
	auto parser = LuaParser::LoadFromBuffer(std::move(source));

	parser->BuildAstWithComment();

	auto errors = parser->GetErrors();

	for (auto& err : errors)
	{
		std::cout << format("error: {} , textRange({},{})", err.ErrorMessage, err.ErrorRange.StartOffset,
		                    err.ErrorRange.EndOffset) << std::endl;
	}

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
	std::cout << "格式化结果:\n";
	std::cout << formatter.GetFormattedText();

	return 0;
}
