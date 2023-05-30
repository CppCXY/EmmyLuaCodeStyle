#include <iostream>
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Parse/LuaParser.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Format/FormatBuilder.h"
#include "CodeService/Config/EditorconfigPattern.h"
#include "CodeService/Diagnostic/DiagnosticBuilder.h"

int main() {
    std::string buffer = R"(
map(1231, 3131, 12331113131,1 )
map(213112, 1, 12313131,1313113131)
)";

    auto file = std::make_shared<LuaFile>(std::move(buffer));
    LuaLexer luaLexer(file);
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    LuaSyntaxTree t;
    t.BuildTree(p);
    std::cout << t.GetDebugView() << std::endl;

    LuaStyle s;
    s.align_continuous_similar_call_args = true;
    FormatBuilder b(s);
    auto text = b.GetFormatResult(t);
    std::cout<< text << std::endl;
//    LuaDiagnosticStyle style;
//    DiagnosticBuilder dd(s, style);
//    dd.CodeStyleCheck(t);
//    for (auto &diag: dd.GetDiagnosticResults(t)) {
//        std::cout << diag.Message << std::endl;
//    }
    return 0;
}