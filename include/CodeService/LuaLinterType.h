#pragma once

#include "LuaCodeStyleEnum.h"
#include "NameStyle/NameStyleRuleMatcher.h"
#include <nlohmann/json_fwd.hpp>
#include "LinterDescription.h"
#include "FormatElement/FormatElementEnum.h"
#include "LuaParser/LuaAstNode/LuaAstNode.h"

enum class EnableType
{
	None,
	Enable
};

class FormatElement;

class BasicLinterStyle
{
public:
	explicit BasicLinterStyle(EnableType enable);

	EnableType Enable;

	virtual void Deserialize(nlohmann::json json) = 0;

	virtual operator bool();
};

class LinterMaxLineLength: public BasicLinterStyle
{
public:
	LinterMaxLineLength();
	int Value;

	void Deserialize(nlohmann::json json) override;
};

class LinterNoIndentStyle: public BasicLinterStyle
{
public:
	LinterNoIndentStyle();
	bool DoStatement;
	bool Label;
	bool IfCondition;

	void Deserialize(nlohmann::json json) override;
};

class LinterIndentStyle : public BasicLinterStyle
{
public:
	LinterIndentStyle();

	IndentStyle Style;
	int IndentSize;
	int TabWidth;
	LinterNoIndentStyle NoIndent;

	void Deserialize(nlohmann::json json) override;
};

class LinterQuoteStyle : public BasicLinterStyle
{
public:
	LinterQuoteStyle();

	QuoteStyle Style;

	void Deserialize(nlohmann::json json) override;
};

class LinterAlignStyle : public BasicLinterStyle
{
public:
	LinterAlignStyle();

	int MaxContinuousLineDistance;
	AlignStyleLevel LocalOrAssign;
	AlignStyleLevel IfCondition;
	AlignStyleLevel TableField;

	void Deserialize(nlohmann::json json) override;
};

class LinterWhiteSpace : public BasicLinterStyle
{
public:
	LinterWhiteSpace();

	TextSpaceType CalculateSpace(std::shared_ptr<LuaAstNode> node);

	std::set<std::string, std::less<>> WrapperSpace;
	std::set<std::string, std::less<>> NoWrapperSpace;
	std::set<std::string, std::less<>> AfterSpace;

	void Deserialize(nlohmann::json json) override;
};

class LinterDetailLineSpace : public BasicLinterStyle
{
public:
	LinterDetailLineSpace(LinterDescription description);

	LinterDescription Description;
	std::shared_ptr<FormatElement> Element;

	void Deserialize(nlohmann::json json) override;
};


class LinterLineSpace : public BasicLinterStyle
{
public:
	LinterLineSpace();

	LinterDetailLineSpace keep_line_after_if_statement;
	LinterDetailLineSpace keep_line_after_do_statement;
	LinterDetailLineSpace keep_line_after_while_statement;
	LinterDetailLineSpace keep_line_after_repeat_statement;
	LinterDetailLineSpace keep_line_after_for_statement;
	LinterDetailLineSpace keep_line_after_local_or_assign_statement;
	LinterDetailLineSpace keep_line_after_function_define_statement;

	void Deserialize(nlohmann::json json) override;
};

class LinterNameStyle : public BasicLinterStyle
{
public:
	LinterNameStyle();

	std::shared_ptr<NameStyleRuleMatcher> local_name_define_style;
	std::shared_ptr<NameStyleRuleMatcher> function_param_name_style;
	std::shared_ptr<NameStyleRuleMatcher> function_name_define_style;
	std::shared_ptr<NameStyleRuleMatcher> local_function_name_define_style;
	std::shared_ptr<NameStyleRuleMatcher> table_field_name_define_style;
	std::shared_ptr<NameStyleRuleMatcher> global_variable_name_define_style;
	std::shared_ptr<NameStyleRuleMatcher> module_name_define_style;
	std::shared_ptr<NameStyleRuleMatcher> require_module_name_style;
	std::shared_ptr<NameStyleRuleMatcher> class_name_define_style;

	void Deserialize(nlohmann::json json) override;
};
