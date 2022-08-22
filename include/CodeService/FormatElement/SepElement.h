#pragma once

#include "TextElement.h"

class SepElement : public TextElement
{
public:
	SepElement();

	explicit SepElement(std::shared_ptr<LuaAstNode> node);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void SetEmpty();
private:
	bool _empty;
};
