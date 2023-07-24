#include "FormatService.h"

//#include "CodeActionService.h"
//#include "CommandService.h"
//#include "CodeFormatCore/LuaFormatter.h"
//#include "CodeFormatCore/FormatElement/DiagnosisContext.h"
//#include "CodeFormatCore/NameStyle/NameStyleChecker.h"
#include "LanguageServer.h"
#include "CodeFormatCore/Format/FormatBuilder.h"
#include "CodeFormatCore/RangeFormat/RangeFormatBuilder.h"


FormatService::FormatService(LanguageServer *owner)
        : Service(owner) {
}

std::string FormatService::Format(LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle) {
    FormatBuilder f(luaStyle);
    return f.GetFormatResult(luaSyntaxTree);
}

std::string FormatService::RangeFormat(LuaSyntaxTree &luaSyntaxTree, LuaStyle &luaStyle, FormatRange &range) {
    RangeFormatBuilder f(luaStyle, range);
    auto text = f.GetFormatResult(luaSyntaxTree);
    range = f.GetReplaceRange();
    return text;
}

std::vector<LuaTypeFormat::Result>
FormatService::TypeFormat(std::string_view trigger,
                          std::size_t line,
                          std::size_t character,
                          LuaSyntaxTree &luaSyntaxTree,
                          LuaStyle &luaStyle,
                          LuaTypeFormatFeatures &typeOptions) {
    LuaTypeFormat tf(typeOptions);
    tf.Analyze(trigger, line, character, luaSyntaxTree, luaStyle);
    return tf.GetResult();
}
