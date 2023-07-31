#include <iostream>
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Parse/LuaParser.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeFormatCore/Format/FormatBuilder.h"
#include "CodeFormatCore/Config/EditorconfigPattern.h"
#include "CodeFormatCore/Diagnostic/DiagnosticBuilder.h"

int main() {
    std::string buffer = R"(
local t = { function ()
    local t = 123
end }

local t = { {
    okokok = 123
} }
)";

    auto file = std::make_shared<LuaSource>(std::move(buffer));
    LuaLexer luaLexer(file);
    luaLexer.SupportNonStandardSymbol();
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    LuaSyntaxTree t;
    t.BuildTree(p);
    std::cout << t.GetDebugView() << std::endl;

    LuaStyle s;
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