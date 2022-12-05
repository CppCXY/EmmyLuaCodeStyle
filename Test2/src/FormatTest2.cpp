#include <iostream>
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Parse/LuaParser.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Format/FormatBuilder.h"

int main() {
    std::string buffer = R"(
local t = {
    [ [[哎哟你干嘛啊]] .. 1 --[[afafa]] ] = 123
}

)";

    auto file = std::make_shared<LuaFile>(std::move(buffer));
    LuaLexer luaLexer(file);
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    LuaSyntaxTree t;
    t.BuildTree(p);
    LuaStyle s;
    s.end_of_line = EndOfLine::LF;
    FormatBuilder b(s);
    b.FormatAnalyze(t);
    std::cout<<t.GetDebugView()<<std::endl;
    auto text = b.GetFormatResult(t);
    std::cout<< text << std::endl;
    return 0;
}