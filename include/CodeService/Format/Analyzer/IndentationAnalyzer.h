#pragma once

#include <stack>
#include <optional>
#include <unordered_set>
#include <unordered_map>
#include "CodeService/Format/Analyzer/FormatAnalyzer.h"
#include "CodeService/Config/LuaStyleEnum.h"
#include "CodeService/Format/Types.h"

/*
 * 缩进的复杂性在于, 除了正常的语句缩进, 在普遍的审美里面,
 * 缩进应当取决于之前的元素是否缩进了, 以及可能会发生的行打断导致的缩进,
 * 缩进操作必须基于序列化时的情况来分析, 所以缩进策略里面会有很多延后判断
 */
class IndentationAnalyzer : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(IndentationAnalyzer)

    IndentationAnalyzer();

    void Analyze(FormatState &f, const LuaSyntaxTree &t) override;

    void Query(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

    void Indenter(LuaSyntaxNode &n, const LuaSyntaxTree &t, IndentData indentData = IndentData());

    // 在格式化过程中标记Token缩进
    void MarkIndent(LuaSyntaxNode &n, const LuaSyntaxTree &t);

private:
    void AnalyzeExprList(FormatState &f, LuaSyntaxNode &exprList, const LuaSyntaxTree &t);

    void AnalyzeCallExprList(FormatState &f, LuaSyntaxNode &exprList, const LuaSyntaxTree &t);

    void AnalyzeTableFieldKeyValuePairExpr(FormatState &f, LuaSyntaxNode &expr, const LuaSyntaxTree &t);

    bool IsExprShouldIndent(LuaSyntaxNode &expr, const LuaSyntaxTree &t);

    std::unordered_map<std::size_t, IndentData> _indent;
    std::unordered_set<std::size_t> _indentMark;
};



