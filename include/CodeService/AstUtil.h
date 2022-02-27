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

	bool NextMatch(LuaAstNode::ChildIterator it, LuaAstNodeType type, const LuaAstNode::ChildrenContainer& container);

	std::shared_ptr<LuaAstNode> NextNode(LuaAstNode::ChildIterator it, const LuaAstNode::ChildrenContainer& container);
}
