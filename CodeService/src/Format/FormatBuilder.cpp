#include "CodeService/Format/FormatBuilder.h"
#include "Analyzer/SpaceAnalyzer.h"
#include "Analyzer/IndentationAnalyzer.h"
#include "Analyzer/LineBreakAnalyzer.h"

FormatBuilder::FormatBuilder() {

}

void FormatBuilder::FormatAnalyze(const LuaSyntaxTree &t) {
    AddAnalyzer<SpaceAnalyzer>();
    AddAnalyzer<IndentationAnalyzer>();
    AddAnalyzer<LineBreakAnalyzer>();

    auto syntaxNodes = t.GetSyntaxNodes();
    for (const auto &analyzer: _analyzers) {
        for (auto syntaxNode: syntaxNodes) {
            analyzer->Analyze(*this, syntaxNode, t);
        }
    }
}

std::string FormatBuilder::GetFormatResult(const LuaSyntaxTree &t) {
    std::string result;
    auto tokens = t.GetTokens();
    for (auto token: tokens) {
        for(auto& analyzer: _analyzers){
            analyzer->Process(*this, token, t);
        }
        result.append(token.GetText(t));
    }

    return result;
}

