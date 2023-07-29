#pragma once

#include "CodeFormatCore/Config/LuaStyle.h"
#include "FormatState.h"
#include "LuaParser/Ast/LuaSyntaxNode.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include <array>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

class FormatBuilder {
public:
    explicit FormatBuilder(LuaStyle &style);

    virtual std::string GetFormatResult(const LuaSyntaxTree &t);

protected:
    void DoResolve(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve);

    virtual void WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    virtual void WriteSpace(std::size_t space);

    void AddEndOfLine(std::size_t line);

    virtual void WriteLine(std::size_t line);

    virtual void WriteIndent();

    virtual void WriteChar(char ch);

    virtual void WriteText(std::string_view text);

    void DealEndWithNewLine(bool newLine);

    FormatState _state;
    std::string _formattedText;
};
