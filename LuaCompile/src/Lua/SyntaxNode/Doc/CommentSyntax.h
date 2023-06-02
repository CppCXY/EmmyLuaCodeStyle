#include "DocBaseSyntax.h"
#include "DocTagSyntax.h"

class CommentSyntax : public DocBaseSyntax {
public:
    static bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::Comment;
    }

    explicit CommentSyntax(LuaNodeOrToken n = LuaNodeOrToken());

    std::vector<DocTagSyntax> GetTags(const LuaSyntaxTree &t) const;
};
