#pragma once

#include "LuaParser/LuaAstNode.h"
#include "FormatElementType.h"
#include "FormatContext.h"

class FormatElement
{
public:
	explicit  FormatElement(TextRange range);
	virtual ~FormatElement();

	virtual FormatElementType GetType() = 0;

	virtual TextRange GetTextRange();

	virtual void AddChild(std::shared_ptr<LuaAstNode> node);

	virtual void AddChild(std::shared_ptr<FormatElement> child);

	virtual void Serialize(FormatContext& ctx);

protected:
	void AddTextRange(TextRange range);

	TextRange _textRange;
};
