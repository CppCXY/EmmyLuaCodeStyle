#include <iostream>
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Parse/LuaParser.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeFormatCore/Format/FormatBuilder.h"
#include "CodeFormatCore/Config/EditorconfigPattern.h"
#include "CodeFormatCore/Diagnostic/DiagnosticBuilder.h"

int main() {
    std::string buffer = R"(
local t = function()  local a = 131


    local b = function()
        local c = {
        }
    end
end

local t2 = function()
    local c = 1
end
)";

    auto file = std::make_shared<LuaFile>(std::move(buffer));
    LuaLexer luaLexer(file);
    luaLexer.SupportNonStandardSymbol();
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    LuaSyntaxTree t;
    t.BuildTree(p);
    std::cout << t.GetDebugView() << std::endl;

    LuaStyle s;
    s.indent_style = IndentStyle::Tab;
    s.keep_indents_on_empty_lines = true;
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
