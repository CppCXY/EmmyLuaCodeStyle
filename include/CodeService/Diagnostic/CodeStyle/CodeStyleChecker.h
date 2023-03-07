#pragma once

#include <vector>
#include <map>
#include <string>
#include <set>
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Config/LuaDiagnosticStyleEnum.h"
#include "CodeService/Config/LuaStyle.h"
#include "CodeService/Format/FormatState.h"

class DiagnosticBuilder;

class CodeStyleChecker {
public:
    CodeStyleChecker();

    void Analyze(DiagnosticBuilder &d, const LuaSyntaxTree &t);

private:
    void BasicStyleCheck(DiagnosticBuilder &d, const LuaSyntaxTree &t);

    void
    BasicResolve(LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve, DiagnosticBuilder &d);

    void ProcessSpaceDiagnostic(LuaSyntaxNode &node, LuaSyntaxNode &next,
                                size_t shouldSpace,
                                const LuaSyntaxTree &t,
                                DiagnosticBuilder &d
                                );

    void ProcessIndentDiagnostic(LuaSyntaxNode &node,
                                const LuaSyntaxTree &t,
                                DiagnosticBuilder &d
    );

    std::string GetIndentNote(IndentState indent, IndentStyle style);

    std::string GetAdditionalNote(LuaSyntaxNode &left, LuaSyntaxNode &right, const LuaSyntaxTree &t);

    void EndWithNewLine(DiagnosticBuilder &d, const LuaSyntaxTree &t);
};