#pragma once

#include "CodeFormatCore/Config/LuaStyleEnum.h"
#include "FormatAnalyzer.h"
#include <optional>
#include <stack>
#include <unordered_map>
#include <unordered_set>

/*
 * 缩进的复杂性在于, 除了正常的语句缩进, 在普遍的审美里面,
 * 缩进应当取决于之前的元素是否缩进了, 以及可能会发生的行打断导致的缩进,
 * 缩进操作必须基于序列化时的情况来分析, 所以缩进策略里面会有很多延后判断
 */
class IndentationAnalyzer : public FormatAnalyzer {
public:
    DECLARE_FORMAT_ANALYZER(IndentationAnalyzer)

    struct WaitLinebreakGroup {
        std::vector<LuaSyntaxNode> TriggerNodes;
        std::size_t Indent = 0;
        LuaSyntaxNode Parent;
    };

    IndentationAnalyzer();

    void Analyze(FormatState &f, const LuaSyntaxTree &t) override;

    void Query(FormatState &f, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) override;

    void AddIndenter(LuaSyntaxNode n, const LuaSyntaxTree &t, IndentData indentData = IndentData());

    void AddLinebreakGroup(LuaSyntaxNode parent, std::vector<LuaSyntaxNode> &group, const LuaSyntaxTree &t, std::size_t indent);

    // 在格式化过程中标记Token缩进
    void MarkIndent(LuaSyntaxNode n, const LuaSyntaxTree &t);

    void OnFormatMessage(FormatState &f, FormatEvent event, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t) override;

private:
    void AnalyzeExprList(FormatState &f, LuaSyntaxNode &exprList, const LuaSyntaxTree &t);

    void AnalyzeCallExprList(FormatState &f, LuaSyntaxNode &exprList, const LuaSyntaxTree &t);

    void AnalyzeTableFieldKeyValuePairExpr(FormatState &f, LuaSyntaxNode &expr, const LuaSyntaxTree &t);

    bool IsExprShouldIndent(LuaSyntaxNode &expr, const LuaSyntaxTree &t);

    void ProcessExceedLinebreak(FormatState &f, LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t);

    std::unordered_map<std::size_t, IndentData> _indent;
    std::unordered_set<std::size_t> _indentMark;

    std::unordered_map<std::size_t, std::size_t> _waitLinebreak;
    std::vector<WaitLinebreakGroup> _waitLinebreakGroups;
};
