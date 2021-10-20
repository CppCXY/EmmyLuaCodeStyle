#pragma once

#include "LuaParser/LuaAstNode.h"
#include "FormatElementType.h"
#include "FormatContext.h"

class FormatElement
{
public:
	explicit FormatElement(TextRange range = TextRange());
	virtual ~FormatElement();

	virtual FormatElementType GetType() = 0;
	/*
	 * 智能指针在多态方面支持不够方便
	 */
	virtual void Serialize(FormatContext& ctx, int position, FormatElement* parent);

	void Format(FormatContext& ctx);

	TextRange GetTextRange();

	void AddChild(std::shared_ptr<FormatElement> child);

	std::vector<std::shared_ptr<FormatElement>>& GetChildren();

	bool HasValidTextRange() const;

	std::shared_ptr<FormatElement> LastValidElement() const;

	template <class T, class ... ARGS>
	void Add(ARGS ...args)
	{
		AddChild(std::make_shared<T>(std::forward<ARGS>(args)...));
	}

protected:
	void AddTextRange(TextRange range);

	TextRange _textRange;

	std::vector<std::shared_ptr<FormatElement>> _children;
};
