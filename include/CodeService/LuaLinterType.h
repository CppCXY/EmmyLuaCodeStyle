#pragma once

#include "LuaCodeStyleEnum.h"
#include "NameStyle/NameStyleRuleMatcher.h"
#include <nlohmann/json_fwd.hpp>

enum class EnableType
{
	None,
	Enable
};

class BasicLinterStyle
{
public:
	explicit BasicLinterStyle(EnableType enable);

	EnableType Enable;

	virtual void Deserialize(nlohmann::json json) = 0;
};

class LinterIndentStyle : public BasicLinterStyle
{
public:
	LinterIndentStyle();

	IndentStyle Style;
	int IndentSize;
	int TabWidth;

	void Deserialize(nlohmann::json json) override;
};

class LinterQuoteStyle : public BasicLinterStyle
{
public:
	LinterQuoteStyle();

	QuoteStyle Style;

	void Deserialize(nlohmann::json json) override;
};

enum class AlignStyleLevel
{
	None,
	Allow,
	Suggest,
	Ban
};


class LinterAlignStyle : public BasicLinterStyle
{
public:
	LinterAlignStyle();

	AlignStyleLevel LocalOrAssign;
	AlignStyleLevel IfCondition;
	AlignStyleLevel TableField;

	void Deserialize(nlohmann::json json) override;
};

class LinterWhiteSpace : public BasicLinterStyle
{
public:
	LinterWhiteSpace();

	void Deserialize(nlohmann::json json) override;
};

class LinterLineSpace : public BasicLinterStyle
{
public:
	LinterLineSpace();

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
