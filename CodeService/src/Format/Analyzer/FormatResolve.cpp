#include <cstdlib>
#include <optional>
#include "CodeService/Format/Analyzer/FormatResolve.h"

FormatResolve::FormatResolve()
        : _spaceStrategy(NextSpaceStrategy::None),
          _tokenStrategy(TokenStrategy::Origin),
          _data() {

}

NextSpaceStrategy FormatResolve::GetNextSpaceStrategy() const {
    return _spaceStrategy;
}

TokenStrategy FormatResolve::GetTokenStrategy() const {
    return _tokenStrategy;
}

void FormatResolve::Reset() {
    _tokenStrategy = TokenStrategy::Origin;
    _spaceStrategy = NextSpaceStrategy::None;
    _data.Space = 0;
}

void FormatResolve::SetNextSpace(std::size_t space) {
    _spaceStrategy = NextSpaceStrategy::Space;
    _data.Space = space;
}

std::size_t FormatResolve::GetNextSpace() {
    return _data.Space;
}

std::size_t FormatResolve::GetNextLine() {
    return _data.Line;
}

void FormatResolve::SetNextLineBreak(std::size_t line) {
    _spaceStrategy = NextSpaceStrategy::LineBreak;
    _data.Line = line;
}

std::size_t FormatResolve::GetIndent() const {
    return _data.Indent;
}

void FormatResolve::SetIndent(std::size_t indent) {
    _spaceStrategy = NextSpaceStrategy::Indent;
    _data.Indent = indent;
}
