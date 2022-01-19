#include "CodeFormatServer/Service/AstUtil/AstPattern.h"

AstPattern::AstPattern(LuaAstNodeType baseType)
{
}

bool AstPattern::Match(std::shared_ptr<LuaAstNode> node)
{
	return true;
}
