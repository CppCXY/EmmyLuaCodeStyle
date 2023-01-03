#include "CodeService/Format/Analyzer/PreferenceAnalyzer.h"
#include "CodeService/Format/FormatBuilder.h"

PreferenceAnalyzer::PreferenceAnalyzer() {

}

void PreferenceAnalyzer::Analyze(FormatBuilder &f, const LuaSyntaxTree &t) {
    if (f.GetStyle().auto_split_or_join_lines) {
        AnalyzeAutoSplitOrJoinLines(f, t);
    }
}

void
PreferenceAnalyzer::Query(FormatBuilder &f, LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {

}

void PreferenceAnalyzer::AnalyzeAutoSplitOrJoinLines(FormatBuilder &f, const LuaSyntaxTree &t) {
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

