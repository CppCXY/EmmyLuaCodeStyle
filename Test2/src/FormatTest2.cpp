#include <iostream>
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Parse/LuaParser.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Format/FormatBuilder.h"
#include "CodeService/Config/EditorconfigPattern.h"

int main() {
    std::string buffer = R"(
p({

})
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
    s.call_arg_parentheses = CallArgParentheses::Remove;
    FormatBuilder b(s);
    auto text = b.GetFormatResult(t);
    std::cout<< text << std::endl;

//    LuaDiagnosticStyle style;
//    StyleDiagnostic d(style);
//    b.Diagnostic(d, t);
//    for (auto &diag: d.GetDiagnosticResults()) {
//        std::cout << diag.Message << std::endl;
//    }
    return 0;
}