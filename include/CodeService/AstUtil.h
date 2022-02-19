#pragma once

#include "LuaParser/LuaAstNode/LuaAstNode.h"
#include "LuaCodeStyleEnum.h"

namespace ast_util
{
	bool IsSingleStringOrTableArg(std::shared_ptr<LuaAstNode> callArgList);

	bool WillCallArgHaveParentheses(std::shared_ptr<LuaAstNode> callArgList, CallArgParentheses callArgParentheses);
}
