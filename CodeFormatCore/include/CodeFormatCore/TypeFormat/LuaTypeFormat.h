#pragma once

#include "CodeFormatCore/Format/FormatBuilder.h"
#include "LuaTypeFormatFeatures.h"

class LuaTypeFormat {
public:
    struct Result {
        FormatRange Range;
        std::string Text;
    };

    explicit LuaTypeFormat(LuaTypeFormatFeatures &features);

    void Analyze(
            std::string_view trigger, std::size_t line, std::size_t character,
            const LuaSyntaxTree &t,
            LuaStyle &style);

    std::vector<Result> GetResult();

private:
    void AnalyzeReturn(std::size_t line, std::size_t character,
                       const LuaSyntaxTree &t,
                       LuaStyle &style);

    void CompleteMissToken(std::size_t line,
                           std::size_t character,
                           const LuaParseError &luaError,
                           const LuaSyntaxTree &t,
                           LuaStyle &style);

    void FormatLine(std::size_t line,
                    std::size_t character,
                    const LuaSyntaxTree &t,
                    LuaStyle &style);

    void FormatByRange(FormatRange range,
                       const LuaSyntaxTree &t,
                       LuaStyle &style);

    void FixIndent(std::size_t line,
                   std::size_t character,
                   const LuaSyntaxTree &t,
                   LuaStyle &style);

    void FixEndIndent(std::size_t line, std::size_t character);

    LuaTypeFormatFeatures _features;
    std::vector<Result> _results;
};
