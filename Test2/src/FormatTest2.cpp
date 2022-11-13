#include <iostream>
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Parse/LuaParser.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Format/FormatBuilder.h"


int main() {
    std::string buffer = R"(
local t= -  1414+789
)";

    auto file = std::make_shared<LuaFile>(std::move(buffer));
    LuaLexer luaLexer(file);
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    LuaSyntaxTree t;
    t.BuildTree(p);
    LuaStyle s;
    FormatBuilder b(s);
    b.FormatAnalyze(t);
    std::cout<<t.GetDebugView()<<std::endl;
    auto text = b.GetFormatResult(t);
    return 0;
}