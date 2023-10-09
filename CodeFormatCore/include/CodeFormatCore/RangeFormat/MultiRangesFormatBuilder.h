#pragma once

#include "CodeFormatCore/Format/FormatBuilder.h"
#include <string>
#include <string_view>
#include <vector>

class MultiRangesFormatBuilder : public FormatBuilder {
public:
    enum class Valid {
        Init,
        Process,
        Finish
    };

    struct RangeResult {
        std::string Text;
        FormatRange Range;
    };

    MultiRangesFormatBuilder(LuaStyle &style, std::vector<FormatRange> &range);

    std::string GetMultiFormatResult(const LuaSyntaxTree &t);

protected:
    void WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) override;

    void WriteSpace(std::size_t space) override;

    void WriteLine(std::size_t line) override;

    void WriteIndent() override;

    void WriteChar(char ch) override;

    void WriteText(std::string_view text) override;

private:
    void CheckRange(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve);

    std::vector<RangeResult> _results;
};
