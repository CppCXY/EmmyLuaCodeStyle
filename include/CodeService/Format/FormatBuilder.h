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

    virtual std::string GetFormatResult(const LuaSyntaxTree &t);
protected:
    void DoResolve(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve);

    virtual void WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t);

    virtual void WriteSpace(std::size_t space);

    virtual void WriteLine(std::size_t line);

    virtual void WriteIndent();

    virtual void WriteChar(char ch);

    virtual void WriteText(std::string_view text);

    void DealEndWithNewLine(bool newLine);

    FormatState _state;
    std::string _formattedText;
};
