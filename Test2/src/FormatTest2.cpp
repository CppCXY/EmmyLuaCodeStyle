#include <iostream>
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Parse/LuaParser.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Format/FormatBuilder.h"

int main() {
    std::string buffer = R"(
local PlayerPrefs = require 'PlayerPrefs'
local PlayerPrefsDefine = require 'PlayerPrefsDefine'
local CommonSettingUi = require 'CommonSettingUi'
local PerformSwitchManager = require 'PerformSwitchManager'
local MapShowDataManager = require 'MapShowDataManager'
require("mobdebug.test23.test")
-- GameAudioUtils
---@type MapMiddleware
local MapMiddleware

local EnableLog = false
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
    auto text = b.GetFormatResult(t);
    std::cout<< text << std::endl;
    return 0;
}