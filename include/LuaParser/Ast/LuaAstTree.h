#pragma once

#include <memory>
#include <vector>
#include "LuaParser/File/LuaFile.h"
#include "LuaParser/Parse/Mark.h"
#include "LuaParser/Lexer/LuaToken.h"

class LuaAstTree {
public:
    LuaAstTree(std::shared_ptr<LuaFile> file,
               std::vector<LuaToken> &&tokens,
               std::vector<MarkEvent> &&events);

    void BuildTree();

private:
    std::shared_ptr<LuaFile> _file;
    std::vector<LuaToken> _tokens;
    std::vector<MarkEvent> _events;
};