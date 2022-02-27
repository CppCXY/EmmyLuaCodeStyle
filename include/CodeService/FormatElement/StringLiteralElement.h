#pragma once


#include "FormatElement.h"

class StringLiteralElement : public FormatElement
{
public:
	explicit StringLiteralElement(std::shared_ptr<LuaAstNode> node);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;

	std::string_view GetText() const;
private:
	bool ExistDel(char del);
	std::string_view _text;
};

