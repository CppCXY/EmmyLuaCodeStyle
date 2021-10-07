#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include "LuaAstNodeType.h"

class LuaAstNode : std::enable_shared_from_this<LuaAstNode>
{
public:
	LuaAstNodeType Type;
	std::string_view Text;
	std::vector<std::shared_ptr<LuaAstNode>> Children;
};
