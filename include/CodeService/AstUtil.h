#pragma once

#include "LuaParser/LuaAstNode/LuaAstNode.h"
#include "LuaCodeStyleEnum.h"

namespace ast_util
{
	bool IsSingleStringOrTableArg(std::shared_ptr<LuaAstNode> callArgList);

	bool IsSingleStringArg(std::shared_ptr<LuaAstNode> callArgList);

	bool IsSingleStringArgUnambiguous(std::shared_ptr<LuaAstNode> callArgList);

	bool IsSingleTableArg(std::shared_ptr<LuaAstNode> callArgList);

	bool WillCallArgHaveParentheses(std::shared_ptr<LuaAstNode> callArgList, CallArgParentheses callArgParentheses);

	std::shared_ptr<LuaAstNode> FindLeftIndexExpression(std::shared_ptr<LuaAstNode> expression);

	bool WillIndexExpressionFormatError(std::shared_ptr<LuaAstNode> expression);
}
