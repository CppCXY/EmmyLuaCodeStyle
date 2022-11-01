#include "CodeService/Format/FormatBuilder.h"
#include "Analyzer/SpaceAnalyzer.h"
#include "Analyzer/IndentationAnalyzer.h"

FormatBuilder::FormatBuilder() {

}

void FormatBuilder::FormatAnalyze(const LuaSyntaxTree &t) {

    AddAnalyzer<SpaceAnalyzer>();
    AddAnalyzer<IndentationAnalyzer>();

    auto syntaxNodes = t.GetSyntaxNodes();
    for (const auto &analyzer: _analyzers) {
        analyzer->Analyze(*this, syntaxNodes, t);
    }

    for (const auto &analyzer: _analyzers) {
        analyzer->Process(*this, syntaxNodes, t);
    }


}

void FormatBuilder::SpaceAround(LuaSyntaxNode &n, std::size_t space) {
    SpaceLeft(n, space);
    SpaceRight(n, space);
}

void FormatBuilder::SpaceLeft(LuaSyntaxNode &n, std::size_t space) {
    _leftSpaces[n.GetIndex()] = space;
}

void FormatBuilder::SpaceRight(LuaSyntaxNode &n, std::size_t space) {
    _rightSpaces[n.GetIndex()] = space;
}

void FormatBuilder::Indenter(LuaSyntaxNode &n) {
    _indentStates[n.GetIndex()] = IndentState();
}

std::optional<std::size_t> FormatBuilder::GetLeftSpace(LuaSyntaxNode &n) {
    auto it = _leftSpaces.find(n.GetIndex());
    if(it != _leftSpaces.end()){
        return it->second;
    }
    return {};
}

std::optional<std::size_t> FormatBuilder::GetRightSpace(LuaSyntaxNode &n) {
    auto it = _rightSpaces.find(n.GetIndex());
    if(it != _rightSpaces.end()){
        return it->second;
    }
    return {};
}

void FormatBuilder::PushChange(FormatChange&& change) {
    _changes.emplace_back(change);
}
