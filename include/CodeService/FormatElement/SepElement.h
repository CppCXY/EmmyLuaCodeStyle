#pragma once

#include "TextElement.h"

class SepElement : public TextElement
{
public:
	static std::shared_ptr<SepElement> After(std::shared_ptr<FormatElement> node);

	SepElement();

	explicit SepElement(std::shared_ptr<LuaAstNode> node);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void SetEmpty();
private:
	bool _empty;
};
