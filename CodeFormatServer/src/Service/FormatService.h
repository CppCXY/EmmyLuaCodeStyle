#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <map>
#include "LSP/LSP.h"
#include "Service.h"
#include "CodeService/Config/LuaStyle.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Format/Types.h"

class FormatService : public Service {
public:
    LANGUAGE_SERVICE(FormatService);

    explicit FormatService(LanguageServer *owner);

    std::string Format(LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle);

    std::string RangeFormat(LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle, FormatRange &range);


};
