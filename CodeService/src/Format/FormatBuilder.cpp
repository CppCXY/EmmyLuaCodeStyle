#include "CodeService/Format/FormatBuilder.h"
#include "Analyzer/SpaceAnalyzer.h"
#include "Analyzer/IndentationAnalyzer.h"

FormatBuilder::FormatBuilder() {

}

void FormatBuilder::FormatAnalyze(const LuaSyntaxTree &t) {

    AddAnalyzer<SpaceAnalyzer>();
    AddAnalyzer<IndentationAnalyzer>();


    auto syntaxNodes = t.GetSyntaxNodes();
    for(auto analyzer: _analyzers){
        analyzer->Analyze(*this, syntaxNodes, t);
    }

    for(auto analyzer: _analyzers){
        analyzer->Process(*this, syntaxNodes, t);
    }

}

void FormatBuilder::SpaceAround(LuaSyntaxNode &n, std::size_t space) {

}

void FormatBuilder::SpaceLeft(LuaSyntaxNode &n, std::size_t space) {

}

void FormatBuilder::SpaceRight(LuaSyntaxNode &n, std::size_t space) {

}
