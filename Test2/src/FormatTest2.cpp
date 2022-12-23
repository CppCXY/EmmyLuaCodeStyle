#include <iostream>
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Parse/LuaParser.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Format/FormatBuilder.h"

int main() {
    std::string buffer = R"(
local t = function () return x() end

ccc=1231313
)";

    auto file = std::make_shared<LuaFile>(std::move(buffer));
    LuaLexer luaLexer(file);
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    LuaSyntaxTree t;
    t.BuildTree(p);
    std::cout<<t.GetDebugView()<<std::endl;

    LuaStyle s;
    FormatBuilder b(s);
    b.FormatAnalyze(t);
    std::size_t start = 3;
    std::size_t end = 4;
    auto text = b.GetRangeFormatResult(start, end, t);
    std::cout<< text << std::endl;
    return 0;
}