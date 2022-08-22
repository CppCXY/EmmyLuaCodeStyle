#pragma once

#include "FormatElement.h"

class TextElement : public FormatElement
{
public:
	explicit TextElement(std::string_view text, TextRange range = TextRange());

	explicit TextElement(std::shared_ptr<LuaAstNode> node);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;

	std::string_view GetText() const;
	/*
	 * 返回值可空
	 */
	std::shared_ptr<LuaAstNode> GetNode() const;
protected:
	std::shared_ptr<LuaAstNode> _node;
	std::string_view _text;
};
