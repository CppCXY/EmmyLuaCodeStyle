#pragma once
#include <memory>
#include <sstream>
#include "LuaParser/LuaParser.h"
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

	std::shared_ptr<FormatElement> FormatLocalStatement(std::shared_ptr<LuaAstNode> localStatement);

	std::shared_ptr<FormatElement> FormatNameDefList(std::shared_ptr<LuaAstNode> nameDefList);

	std::shared_ptr<FormatElement> FormatExpressionList(std::shared_ptr<LuaAstNode> expressionList);

	void DefaultHandle(std::shared_ptr<LuaAstNode> node, std::shared_ptr<FormatElement> envElement);
private:


	std::shared_ptr<LuaParser> _parser;
	// std::shared_ptr<LuaFormatContext> _ctx;
	LuaFormatOptions _options;
	std::vector<LuaCodeDiagnosis> _diagnosis;
	std::shared_ptr<FormatElement> _env;
};
