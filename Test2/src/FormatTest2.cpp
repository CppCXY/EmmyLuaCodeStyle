#include <iostream>
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Parse/LuaParser.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Format/FormatBuilder.h"
#include "ReflectionEnum.h"

int main() {
    std::string buffer = R"(local t = 123)";

    auto file = std::make_shared<LuaFile>(std::move(buffer));
    LuaLexer luaLexer(file);
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    LuaSyntaxTree t;
    t.BuildTree(p);
    auto syntaxs = t.GetSyntaxNodes();
    for (auto n: syntaxs) {
        std::cout << (int)n.GetSyntaxKind(t) << "   " << n.GetText(t) << std::endl;
    }

    return 0;
}