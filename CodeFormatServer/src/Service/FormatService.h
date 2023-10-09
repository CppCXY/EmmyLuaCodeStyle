#pragma once

#include "CodeFormatCore/Config/LuaStyle.h"
#include "CodeFormatCore/Format/Types.h"
#include "CodeFormatCore/TypeFormat/LuaTypeFormat.h"
#include "LSP/LSP.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "Service.h"
#include <map>
#include <string>
#include <string_view>
#include <vector>

class FormatService : public Service {
public:
    LANGUAGE_SERVICE(FormatService);

    explicit FormatService(LanguageServer *owner);

    std::string Format(LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle);

    std::string RangeFormat(LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle, FormatRange &range);

    std::vector<LuaTypeFormat::Result> TypeFormat(
            std::string_view trigger,
            std::size_t line,
            std::size_t character,
            LuaSyntaxTree &luaSyntaxTree,
            LuaStyle &luaStyle,
            LuaTypeFormatFeatures &typeOptions);
};
