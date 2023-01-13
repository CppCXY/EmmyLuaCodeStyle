#include "CodeService/Diagnostic/NameStyle/NameStyleChecker.h"
#include <algorithm>
#include "CodeService/Diagnostic/DiagnosticBuilder.h"
#include "Util/format.h"
#include "CodeService/Config/LanguageTranslator.h"
#include "CodeService/Diagnostic/NameStyle/NameStyleRuleMatcher.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

std::set<std::string, std::less<>> NameStyleChecker::TableFieldSpecialName = {
        "__add", "__sub", "__mul", "__div", "__mod", "__pow",
        "__unm", "__idiv", "__band", "__bor", "__bxor", "__bnot", "__shl",
        "__shr", "__concat", "__len", "__eq", "__lt", "__index", "__newindex",
        "__call", "__gc", "__close", "__mode", "__name"
};

std::set<std::string, std::less<>> NameStyleChecker::GlobalSpecialName = {
        "_G", "_ENV"
};

NameStyleChecker::NameStyleChecker() {
}

void NameStyleChecker::RecordLocalVariable(LuaSyntaxNode &n, const LuaSyntaxTree &t) {
    if (!_scopeStack.empty()) {
        _scopeStack.back().insert(std::string(n.GetText(t)));
    }
}

bool NameStyleChecker::CheckGlobal(LuaSyntaxNode &n, const LuaSyntaxTree &t) {
    auto text = n.GetText(t);
    for (auto &scope: _scopeStack) {
        if (scope.find(text) != scope.end()) {
            return false;
        }
    }

    return true;
}

void NameStyleChecker::Analyze(DiagnosticBuilder &d, const LuaSyntaxTree &t) {
    auto body = t.GetRootNode();
    // todo check `do return end`
    // todo check `if return`
    auto retStmt = body.GetChildSyntaxNode(LuaSyntaxNodeKind::ReturnStatement, t);
    if (retStmt.IsNode(t)) {
        auto exprList = retStmt.GetChildSyntaxNode(LuaSyntaxNodeKind::ExpressionList, t);
        auto returnExpr = exprList.GetChildSyntaxNode(LuaSyntaxMultiKind::Expression, t);
        if (returnExpr.GetSyntaxKind(t) == LuaSyntaxNodeKind::NameExpression) {
            auto name = returnExpr.GetChildToken(TK_NAME, t);
            _module = std::string(name.GetText(t));
        }
    }
    EnterScope();
    CheckInBody(body, t);
    ExitScope();

    Diagnostic(d, t);
}

void NameStyleChecker::EnterScope() {
    _scopeStack.emplace_back();
}

void NameStyleChecker::ExitScope() {
    if (!_scopeStack.empty()) {
        _scopeStack.pop_back();
    }
}

void NameStyleChecker::CheckInNode(LuaSyntaxNode &n, const LuaSyntaxTree &t) {
    if (n.IsNull(t)) {
        return;
    }

    for (auto syntaxNode: n.GetChildren(t)) {
        if (syntaxNode.IsNode(t)) {
            switch (syntaxNode.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::ClosureExpression: {
                    EnterScope();
                    auto functionBody = syntaxNode.GetChildSyntaxNode(LuaSyntaxNodeKind::FunctionBody, t);
                    auto paramList = functionBody.GetChildSyntaxNode(LuaSyntaxNodeKind::ParamList, t);
                    auto params = paramList.GetChildTokens(TK_NAME, t);
                    for (auto paramName: params) {
                        RecordLocalVariable(paramName, t);
                        PushStyleCheck(NameDefineType::ParamName, paramName);
                    }

                    auto body = functionBody.GetChildSyntaxNode(LuaSyntaxNodeKind::Block, t);
                    CheckInBody(body, t);

                    ExitScope();
                    break;
                }
                case LuaSyntaxNodeKind::Block: {
                    EnterScope();

                    CheckInBody(syntaxNode, t);

                    ExitScope();
                    break;
                }
                default: {
                    CheckInNode(syntaxNode, t);
                    break;
                }
            }
        }
    }
}

void NameStyleChecker::CheckInBody(LuaSyntaxNode &n, const LuaSyntaxTree &t) {
    for (auto stmt: n.GetChildren(t)) {
        if (stmt.IsNode(t)) {
            switch (stmt.GetSyntaxKind(t)) {
                case LuaSyntaxNodeKind::LocalStatement: {
                    auto nameDefList = stmt.GetChildSyntaxNode(LuaSyntaxNodeKind::NameDefList, t);
                    auto exprList = stmt.GetChildSyntaxNode(LuaSyntaxNodeKind::ExpressionList, t);
                    CheckInNode(exprList, t);

                    auto names = nameDefList.GetChildTokens(TK_NAME, t);
                    for (auto n: names) {
                        RecordLocalVariable(n, t);
                    }

                    auto exprs = exprList.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);
                    for (std::size_t i = 0; i != names.size(); i++) {
                        auto name = names[i];
                        bool matchSpecialRule = false;
                        if (i < exprs.size()) {
                            auto expr = exprs[i];
                            matchSpecialRule = CheckSpecialVariableRule(name, expr, t);
                        }
                        if (!matchSpecialRule) {
                            if (_scopeStack.size() == 1 && _module == name.GetText(t)) {
                                PushStyleCheck(NameDefineType::ModuleDefineName, name);
                                break;
                            }
                            PushStyleCheck(NameDefineType::LocalVariableName, name);
                        }
                    }

                    break;
                }
                case LuaSyntaxNodeKind::AssignStatement: {
                    auto varList = stmt.GetChildSyntaxNode(LuaSyntaxNodeKind::VarList, t);
                    auto exprList = stmt.GetChildSyntaxNode(LuaSyntaxNodeKind::ExpressionList, t);
                    CheckInNode(exprList, t);

                    auto defineExprs = varList.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);

                    auto exprs = exprList.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t);
                    for (std::size_t i = 0; i != defineExprs.size(); i++) {
                        auto defineExpr = defineExprs[i];
                        bool isNameDefine = defineExpr.GetSyntaxKind(t) == LuaSyntaxNodeKind::NameExpression;
                        if (isNameDefine) {
                            defineExpr = defineExpr.GetChildToken(TK_NAME, t);
                        } else {
                            // table field define
                            auto children = defineExpr.GetChildren(t);
                            if (!children.empty()) {
                                defineExpr = children.back().GetChildToken(TK_NAME, t);
                            }
                        }

                        bool matchSpecialRule = false;
                        if (i < exprs.size()) {
                            auto expr = exprs[i];
                            matchSpecialRule = CheckSpecialVariableRule(defineExpr, expr, t);
                        }

                        if (!matchSpecialRule) {
                            if (isNameDefine && CheckGlobal(defineExpr, t)) {
                                PushStyleCheck(NameDefineType::GlobalVariableDefineName, defineExpr);
                            } else if (!isNameDefine) {
                                PushStyleCheck(NameDefineType::TableFieldDefineName, defineExpr);
                            }
                        }
                    }

                    break;
                }
                case LuaSyntaxNodeKind::ForStatement: {
                    auto forNumber = stmt.GetChildSyntaxNode(LuaSyntaxNodeKind::ForNumber, t);
                    LuaSyntaxNode forBody(0);
                    if (forNumber.IsNode(t)) {
                        for (auto expr: forNumber.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t)) {
                            CheckInNode(expr, t);
                        }

                        EnterScope();

                        auto indexName = forNumber.GetChildToken(TK_NAME, t);
                        RecordLocalVariable(indexName, t);
                        forBody = forNumber.GetChildSyntaxNode(LuaSyntaxNodeKind::ForBody, t);
                    } else {
                        auto forList = stmt.GetChildSyntaxNode(LuaSyntaxNodeKind::ForList, t);
                        auto exprList = forList.GetChildSyntaxNode(LuaSyntaxNodeKind::ExpressionList, t);
                        for (auto expr: exprList.GetChildSyntaxNodes(LuaSyntaxMultiKind::Expression, t)) {
                            CheckInNode(expr, t);
                        }

                        EnterScope();

                        auto nameList = forList.GetChildSyntaxNode(LuaSyntaxNodeKind::NameDefList, t);
                        for (auto name: nameList.GetChildTokens(TK_NAME, t)) {
                            RecordLocalVariable(name, t);
                        }

                        forBody = forList.GetChildSyntaxNode(LuaSyntaxNodeKind::ForBody, t);
                    }

                    auto body = forBody.GetChildSyntaxNode(LuaSyntaxNodeKind::Block, t);
                    CheckInBody(body, t);

                    ExitScope();
                    break;
                }
                case LuaSyntaxNodeKind::FunctionStatement: {
                    auto functionNameExpr = stmt.GetChildSyntaxNode(LuaSyntaxNodeKind::FunctionNameExpression, t);
                    LuaSyntaxNode functionName(0);
                    bool simpleFunction = false;
                    auto indexExpr = functionNameExpr.GetChildSyntaxNode(LuaSyntaxNodeKind::IndexExpression, t);
                    if (indexExpr.IsNode(t)) {
                        auto indexExprs = functionNameExpr.GetChildSyntaxNodes(LuaSyntaxNodeKind::IndexExpression, t);
                        functionName = indexExprs.back().GetChildToken(TK_NAME, t);
                    } else {
                        functionName = functionNameExpr.GetChildToken(TK_NAME, t);
                        simpleFunction = true;
                    }

                    PushStyleCheck(NameDefineType::FunctionDefineName, functionName);
                    EnterScope();
                    // for recursive
                    if (simpleFunction) {
                        RecordLocalVariable(functionName, t);
                    }

                    auto functionBody = stmt.GetChildSyntaxNode(LuaSyntaxNodeKind::FunctionBody, t);
                    auto paramList = functionBody.GetChildSyntaxNode(LuaSyntaxNodeKind::ParamList, t);
                    auto params = paramList.GetChildTokens(TK_NAME, t);
                    for (auto paramName: params) {
                        RecordLocalVariable(paramName, t);
                        PushStyleCheck(NameDefineType::ParamName, paramName);
                    }

                    auto body = functionBody.GetChildSyntaxNode(LuaSyntaxNodeKind::Block, t);
                    CheckInBody(body, t);

                    ExitScope();
                    break;
                }
                case LuaSyntaxNodeKind::LocalFunctionStatement: {
                    auto functionName = stmt.GetChildToken(TK_NAME, t);
                    PushStyleCheck(NameDefineType::LocalFunctionName, functionName);
                    EnterScope();
                    // for recursive
                    RecordLocalVariable(functionName, t);

                    auto functionBody = stmt.GetChildSyntaxNode(LuaSyntaxNodeKind::FunctionBody, t);
                    auto paramList = functionBody.GetChildSyntaxNode(LuaSyntaxNodeKind::ParamList, t);
                    auto params = paramList.GetChildTokens(TK_NAME, t);
                    for (auto paramName: params) {
                        RecordLocalVariable(paramName, t);
                        PushStyleCheck(NameDefineType::ParamName, paramName);
                    }

                    auto body = functionBody.GetChildSyntaxNode(LuaSyntaxNodeKind::Block, t);
                    CheckInBody(body, t);

                    ExitScope();
                    break;
                }
                default: {
                    CheckInNode(stmt, t);
                    break;
                }
            }
        }
    }
}

void NameStyleChecker::PushStyleCheck(NameDefineType type, LuaSyntaxNode &n) {
    _nameStyleCheckVector.emplace_back(type, n.GetIndex());
}

bool NameStyleChecker::CheckSpecialVariableRule(LuaSyntaxNode &define, LuaSyntaxNode &expr, const LuaSyntaxTree &t) {
    if (expr.GetSyntaxKind(t) != LuaSyntaxNodeKind::SuffixedExpression) {
        return false;
    }

    auto childrens = expr.GetChildren(t);
    if (childrens.empty()) {
        return false;
    }

    if (childrens.back().GetSyntaxKind(t) != LuaSyntaxNodeKind::CallExpression) {
        return false;
    }

    auto prevSyntaxNode = childrens.back().GetPrevSibling(t);
    switch (prevSyntaxNode.GetSyntaxKind(t)) {
        case LuaSyntaxNodeKind::IndexExpression:
        case LuaSyntaxNodeKind::NameExpression: {
            auto callName = prevSyntaxNode.GetChildToken(TK_NAME, t);
            auto callNameText = callName.GetText(t);
            if (callNameText == "class" || callNameText == "Class") {
                // class rule
                PushStyleCheck(NameDefineType::ClassVariableName, define);
                return true;
            } else if (callNameText == "require" || callNameText == "import" || callNameText == "Import") {
                // module import rule
                PushStyleCheck(NameDefineType::ImportModuleName, define);
                return true;
            }
            // fall through
        }
        default: {
            return false;
        }
    }

    return false;
}

void NameStyleChecker::Diagnostic(DiagnosticBuilder &d, const LuaSyntaxTree &t) {
    NameStyleRuleMatcher matcher;
    auto &state = d.GetState();
    for (auto &nameStyle: _nameStyleCheckVector) {
        auto n = LuaSyntaxNode(nameStyle.Index);
        switch (nameStyle.Type) {
            case NameDefineType::LocalVariableName: {
                if (!matcher.Match(n, t, state.GetDiagnosticStyle().local_name_style)) {
                    d.PushDiagnostic(DiagnosticType::NameStyle,
                                     n.GetTextRange(t),
                                     MakeDiagnosticInfo("LocalVariableName", n, t,
                                                        state.GetDiagnosticStyle().local_name_style)
                    );
                }
                break;
            }
            case NameDefineType::ModuleDefineName: {
                if (!matcher.Match(n, t, state.GetDiagnosticStyle().module_name_style)) {
                    d.PushDiagnostic(DiagnosticType::NameStyle,
                                     n.GetTextRange(t),
                                     MakeDiagnosticInfo("ModuleName", n, t,
                                                        state.GetDiagnosticStyle().module_name_style)
                    );
                }
                break;
            }
            case NameDefineType::LocalFunctionName: {
                if (TableFieldSpecialName.count(n.GetText(t))) {
                    return;
                }
                if (!matcher.Match(n, t, state.GetDiagnosticStyle().local_function_name_style)) {
                    d.PushDiagnostic(DiagnosticType::NameStyle,
                                     n.GetTextRange(t),
                                     MakeDiagnosticInfo("LocalFunctionName", n, t,
                                                        state.GetDiagnosticStyle().local_function_name_style)
                    );
                }
                break;
            }
            case NameDefineType::GlobalVariableDefineName: {
                if (GlobalSpecialName.count(n.GetText(t))) {
                    return;
                }
                if (!matcher.Match(n, t, state.GetDiagnosticStyle().global_variable_name_style)) {
                    d.PushDiagnostic(DiagnosticType::NameStyle,
                                     n.GetTextRange(t),
                                     MakeDiagnosticInfo("GlobalVariableDefineName", n, t,
                                                        state.GetDiagnosticStyle().global_variable_name_style)
                    );
                }
                break;
            }
            case NameDefineType::ParamName: {
                if (!matcher.Match(n, t, state.GetDiagnosticStyle().function_param_name_style)) {
                    d.PushDiagnostic(DiagnosticType::NameStyle,
                                     n.GetTextRange(t),
                                     MakeDiagnosticInfo("ParamName", n, t,
                                                        state.GetDiagnosticStyle().function_param_name_style)
                    );
                }
                break;
            }
            case NameDefineType::ImportModuleName: {
                if (!matcher.Match(n, t, state.GetDiagnosticStyle().require_module_name_style)) {
                    d.PushDiagnostic(DiagnosticType::NameStyle,
                                     n.GetTextRange(t),
                                     MakeDiagnosticInfo("ImportModuleName", n, t,
                                                        state.GetDiagnosticStyle().require_module_name_style)
                    );
                }
                break;
            }
            case NameDefineType::ClassVariableName: {
                if (!matcher.Match(n, t, state.GetDiagnosticStyle().class_name_style)) {
                    d.PushDiagnostic(DiagnosticType::NameStyle,
                                     n.GetTextRange(t),
                                     MakeDiagnosticInfo("ClassVariableName", n, t,
                                                        state.GetDiagnosticStyle().class_name_style)
                    );
                }
                break;
            }
            case NameDefineType::FunctionDefineName: {
                if (TableFieldSpecialName.count(n.GetText(t))) {
                    return;
                }

                if (!matcher.Match(n, t, state.GetDiagnosticStyle().function_name_style)) {
                    d.PushDiagnostic(DiagnosticType::NameStyle,
                                     n.GetTextRange(t),
                                     MakeDiagnosticInfo("FunctionDefineName", n, t,
                                                        state.GetDiagnosticStyle().function_name_style)
                    );
                }
                break;
            }
            case NameDefineType::TableFieldDefineName: {
                // 忽略元方法
                if (TableFieldSpecialName.count(n.GetText(t))) {
                    return;
                }
                if (!matcher.Match(n, t, state.GetDiagnosticStyle().table_field_name_style)) {
                    d.PushDiagnostic(DiagnosticType::NameStyle,
                                     n.GetTextRange(t),
                                     MakeDiagnosticInfo("TableFieldDefineName", n, t,
                                                        state.GetDiagnosticStyle().table_field_name_style)
                    );
                }
                break;
            }
        }
    }
}

std::string NameStyleChecker::MakeDiagnosticInfo(std::string_view ruleName, LuaSyntaxNode &n, const LuaSyntaxTree &t,
                                                 const std::vector<NameStyleRule> &rules) {
    auto name = n.GetText(t);
    std::string ruleMessage = "";
    for (std::size_t index = 0; index < rules.size(); index++) {
        auto &rule = rules[index];
        switch (rule.Type) {
            case NameStyleType::SnakeCase: {
                ruleMessage.append("snake-case");
                break;
            }
            case NameStyleType::CamelCase: {
                ruleMessage.append("camel-case");
                break;
            }
            case NameStyleType::PascalCase: {
                ruleMessage.append("pascal-case");
                break;
            }
            case NameStyleType::Same: {
                ruleMessage.append(util::format("same('{}')", rule.Param));
                break;
            }
            case NameStyleType::Pattern: {
                ruleMessage.append(util::format("pattern('{}')", rule.Param));
                break;
            }
            case NameStyleType::UpperSnakeCase: {
                ruleMessage.append("upper-snake-case");
                break;
            }
            default: {
                break;
            }
        }
        if (index != rules.size() - 1) {
            ruleMessage.append(" | ");
        }
    }
    return util::format("Name '{}' does not match rule '{}', which require '{}'", name, ruleName, ruleMessage);
}
