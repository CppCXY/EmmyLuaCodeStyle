#include "BreakStmtSyntax.h"

BreakStmtSyntax::BreakStmtSyntax(LuaNodeOrToken n) : LuaBaseSyntax(n) {
}

StmtSyntax BreakStmtSyntax::GetLoopStmt(const LuaSyntaxTree &t) const {
    auto loop = _node.Ancestor(t, [](LuaSyntaxNodeKind kind, bool &ct) -> bool {
        switch (kind) {
            case LuaSyntaxNodeKind::ForNumberStatement:
            case LuaSyntaxNodeKind::ForRangeStatement:
            case LuaSyntaxNodeKind::WhileStatement:
            case LuaSyntaxNodeKind::RepeatStatement: {
                return true;
            }
            case LuaSyntaxNodeKind::FunctionBody: {
                ct = false;
                break;
            }
            default: {
                break;
            }
        }
        return false;
    });
    return StmtSyntax(loop);
}
