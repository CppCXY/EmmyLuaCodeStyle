#pragma once
#include <memory>
#include <sstream>
#include "LuaParser/LuaParser.h"
#include "LuaFormatContext.h"
#include "LuaFormatOptions.h"
#include "LuaCodeDiagnosis.h"
#include "FormatElement/FormatElement.h"

class LuaFormatter
{
public:
	LuaFormatter(std::shared_ptr<LuaParser> luaParser, LuaFormatOptions& options);
	~LuaFormatter() = default;

	void BuildFormattedElement();

	std::string GetFormattedText();

	void AddDiagnosis(TextRange range, std::string_view message);

	std::vector<LuaCodeDiagnosis>& GetDiagnosis();

protected:
	std::shared_ptr<FormatElement> FormatNode(std::shared_ptr<LuaAstNode> node);

	std::shared_ptr<FormatElement> FormatBlock(std::shared_ptr<LuaAstNode> blockNode);




private:
	bool nextMatch(int currentIndex, LuaAstNodeType type ,std::vector<std::shared_ptr<LuaAstNode>> vec);

	std::shared_ptr<LuaParser> _parser;
	std::shared_ptr<LuaFormatContext> _ctx;
	LuaFormatOptions _options;
	std::vector<LuaCodeDiagnosis> _diagnosis;
};
