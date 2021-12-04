#pragma once

#include "FormatElement.h"
#include "LuaParser/LuaAstNode.h"
#include "TextDefineType.h"

class TextElement : public FormatElement
{
public:
	explicit TextElement(std::string_view text, TextDefineType textDefineType, TextRange range = TextRange());

	explicit TextElement(std::shared_ptr<LuaAstNode> node, TextDefineType textDefineType = TextDefineType::Normal);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent) override;

	std::string_view GetText() const;
	void SetTextDefineType(TextDefineType textDefineType);
	
private:
	std::string TranslateNameStyle(NameStyle style);
	bool NameStyleCheck(NameStyle style);
	bool SnakeCase();
	bool PascalCase();
	bool CamelCase();
	std::string_view _text;
	TextDefineType _textDefineType;
};
