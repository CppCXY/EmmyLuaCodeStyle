#pragma once

#include <vector>
#include <map>
#include <string>
#include <set>
#include "NameDefineType.h"
#include "LuaParser/Ast/LuaSyntaxTree.h"
#include "CodeService/Config/LuaDiagnosticStyleEnum.h"

class DiagnosticBuilder;

class NameStyleChecker {
public:
    NameStyleChecker();

    void Analyze(DiagnosticBuilder &d, const LuaSyntaxTree &t);

private:
    static std::set<std::string, std::less<>> TableFieldSpecialName;
    static std::set<std::string, std::less<>> GlobalSpecialName;

    void Diagnostic(DiagnosticBuilder &d, const LuaSyntaxTree &t);

    void CheckInNode(LuaSyntaxNode &n, const LuaSyntaxTree &t);

    void CheckInBody(LuaSyntaxNode &n, const LuaSyntaxTree &t);

    void RecordLocalVariable(LuaSyntaxNode &n, const LuaSyntaxTree &t);

    bool CheckGlobal(LuaSyntaxNode &n, const LuaSyntaxTree &t);

    void EnterScope();

    void ExitScope();

    void PushStyleCheck(NameDefineType type, LuaSyntaxNode &n);

    bool CheckSpecialVariableRule(LuaSyntaxNode &define, LuaSyntaxNode &expr, const LuaSyntaxTree &t);

    std::string MakeDiagnosticInfo(std::string_view ruleName,
                                   LuaSyntaxNode &n,
                                   const LuaSyntaxTree &t,
                                   const std::vector<NameStyleRule> &rules);

    std::string _module;
    std::vector<std::set<std::string, std::less<>>> _scopeStack;
    std::vector<NameStyleInfo> _nameStyleCheckVector;
};
