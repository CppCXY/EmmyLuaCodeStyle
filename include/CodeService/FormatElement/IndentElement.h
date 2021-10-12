#pragma once

#include "FormatElement.h"

class IndentElement : public FormatElement
{
public:
	IndentElement();

	FormatElementType GetType() override;

	void AddChild(std::shared_ptr<FormatElement> child) override;

	void AddChild(std::shared_ptr<LuaAstNode> node) override;

	void KeepLine(int line = -1);

	void MinLine(int line);

private:
	std::vector<FormatElement> _children;
};
