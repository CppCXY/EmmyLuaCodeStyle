#pragma once

#include "CodeService/Format/FormatBuilder.h"

class RangeFormatBuilder : public FormatBuilder {
public:
    RangeFormatBuilder(LuaStyle &style, FormatRange &range);

    std::string GetFormatResult(const LuaSyntaxTree &t) override;

    FormatRange GetReplaceRange() const;

protected:
    void WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) override;

    void WriteSpace(std::size_t space) override;

    void WriteLine(std::size_t line) override;

    void WriteIndent() override;

    void WriteChar(char ch) override;

    void WriteText(std::string_view text) override;

private:
    void CheckRange(LuaSyntaxNode& syntaxNode, const LuaSyntaxTree& t);

    bool _validRange;
    FormatRange _range;
};
