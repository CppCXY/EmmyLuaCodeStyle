#include <cstdlib>
#include "CodeService/Format/Analyzer/FormatResolve.h"

FormatResolve::FormatResolve()
        : _spaceStrategy(SpaceStrategy::None),
          _tokenStrategy(TokenStrategy::Origin),
          _indentStrategy(IndentStrategy::None),
          _spaceData() {

}

SpaceStrategy FormatResolve::GetSpaceStrategy() const {
    return _spaceStrategy;
}

TokenStrategy FormatResolve::GetTokenStrategy() const {
    return _tokenStrategy;
}

void FormatResolve::Reset() {
    _tokenStrategy = TokenStrategy::Origin;
    _spaceStrategy = SpaceStrategy::None;
    _spaceData.Space = 0;
}

void FormatResolve::SetNextSpace(std::size_t space) {
    _spaceStrategy = SpaceStrategy::Space;
    _spaceData.Space = space;
}

std::size_t FormatResolve::GetNextSpace() {
    return _spaceData.Space;
}

std::size_t FormatResolve::GetNextLine() {
    return _spaceData.Line;
}

void FormatResolve::SetNextLineBreak(std::size_t line) {
    _spaceStrategy = SpaceStrategy::LineBreak;
    _spaceData.Line = line;
}

IndentStrategy FormatResolve::GetIndentStrategy() const {
    return _indentStrategy;
}

void FormatResolve::SetIndent(IndentStrategy strategy) {
    _spaceStrategy = SpaceStrategy::Indent;
    _indentStrategy = strategy;
}
