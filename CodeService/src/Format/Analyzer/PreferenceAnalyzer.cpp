#include "CodeService/Format/Analyzer/PreferenceAnalyzer.h"
#include "CodeService/Format/FormatState.h"

PreferenceAnalyzer::PreferenceAnalyzer() {

}

void PreferenceAnalyzer::Analyze(FormatState &f, const LuaSyntaxTree &t) {
    if (f.GetStyle().auto_split_or_join_lines) {
        AnalyzeAutoSplitOrJoinLines(f, t);
    }
}

void
PreferenceAnalyzer::Query(FormatState &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {

}

void PreferenceAnalyzer::AnalyzeAutoSplitOrJoinLines(FormatState &f, const LuaSyntaxTree &t) {
    for (auto node: t.GetSyntaxNodes()) {
        switch (node.GetSyntaxKind(t)) {
            case LuaSyntaxNodeKind::FunctionBody: {

                break;
            }
            case LuaSyntaxNodeKind::CallExpression: {

                break;
            }

        }
    }
}

