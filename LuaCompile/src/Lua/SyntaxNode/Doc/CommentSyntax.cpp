#include "CommentSyntax.h"

CommentSyntax::CommentSyntax(LuaNodeOrToken n) : DocBaseSyntax(n) {
}

std::vector<DocTagSyntax> CommentSyntax::GetTags(const LuaSyntaxTree &t) const {
    return GetMembers<DocTagSyntax>(t);
}
