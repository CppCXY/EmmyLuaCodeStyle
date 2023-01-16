#pragma once

#include <vector>
#include <array>
#include <memory>
#include <unordered_map>
#include <optional>
#include "LuaParser/Ast/LuaSyntaxNode.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Config/LuaStyle.h"
#include "Types.h"
#include "FormatState.h"

class FormatBuilder {
public:
    explicit FormatBuilder(LuaStyle &style);

    void FormatAnalyze(const LuaSyntaxTree &t);

    std::string GetFormatResult(const LuaSyntaxTree &t);

    std::string GetRangeFormatResult(FormatRange &range, const LuaSyntaxTree &t);
private:
    void DoResolve(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve);

    void DoRangeResolve(FormatRange &range, LuaSyntaxNode &syntaxNode,
                        const LuaSyntaxTree &t, FormatResolve &resolve);

    void WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    void WriteSpace(std::size_t space);

    void WriteLine(std::size_t line);

    void WriteIndent();

    void WriteChar(char ch);

    void WriteText(std::string_view text);

    void DealNewLine(bool newLine);

    FormatState _state;
    std::string _formattedText;
};
