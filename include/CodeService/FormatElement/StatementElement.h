#pragma once

#include <vector>
#include "FormatElement.h"
#include "ExpressionElement.h"

class StatementElement : public ExpressionElement
{
public:
	FormatElementType GetType() override;

private:
	std::vector<std::shared_ptr<FormatElement>> _children;
	
};
