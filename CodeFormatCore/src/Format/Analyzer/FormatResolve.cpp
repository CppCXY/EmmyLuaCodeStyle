#include "CodeFormatCore/Format/Analyzer/FormatResolve.h"

FormatResolve::FormatResolve()
    : _nextSpaceStrategy(NextSpaceStrategy::None),
      _prevSpaceStrategy(PrevSpaceStrategy::None),
      _tokenStrategy(TokenStrategy::Origin),
      _nextAddStrategy(NextTokenStrategy::None),
      _indentStrategy(IndentStrategy::None),
      _nextSpaceData(),
      _prevSpaceData(),
      _indent(0) {
}

NextSpaceStrategy FormatResolve::GetNextSpaceStrategy() const {
    return _nextSpaceStrategy;
}

PrevSpaceStrategy FormatResolve::GetPrevSpaceStrategy() const {
    return _prevSpaceStrategy;
}

PrevTokenStrategy FormatResolve::GetPrevTokenStrategy() const {
    return _prevTokenStrategy;
}

TokenStrategy FormatResolve::GetTokenStrategy() const {
    return _tokenStrategy;
}

NextTokenStrategy FormatResolve::GetNextTokenStrategy() const {
    return _nextAddStrategy;
}

IndentStrategy FormatResolve::GetIndentStrategy() const {
    return _indentStrategy;
}

void FormatResolve::Reset() {
    _prevSpaceStrategy = PrevSpaceStrategy::None;
    _prevTokenStrategy = PrevTokenStrategy::None;
    _tokenStrategy = TokenStrategy::Origin;
    _nextAddStrategy = NextTokenStrategy::None;
    _nextSpaceStrategy = NextSpaceStrategy::None;

    _indentStrategy = IndentStrategy::None;
    _nextSpaceData = NextData();
    _prevSpaceData = PrevData();
    _range = IndexRange();
    _indent = 0;
}

void FormatResolve::SetNextSpace(std::size_t space) {
    _nextSpaceStrategy = NextSpaceStrategy::Space;
    _nextSpaceData.Space = space;
}

std::size_t FormatResolve::GetNextSpace() {
    return _nextSpaceData.Space;
}

LineSpace FormatResolve::GetNextLine() {
    return _nextSpaceData.Line;
}

void FormatResolve::SetNextLineBreak(LineSpace line) {
    _nextSpaceStrategy = NextSpaceStrategy::LineBreak;
    _nextSpaceData.Line = line;
}

std::size_t FormatResolve::GetIndent() const {
    return _indent;
}

void FormatResolve::SetIndent(std::size_t indent, IndentStrategy strategy) {
    _indentStrategy = strategy;
    _indent = indent;
}

void FormatResolve::SetAlign(std::size_t alignPos) {
    _prevSpaceStrategy = PrevSpaceStrategy::AlignPos;
    _prevSpaceData.Align = alignPos;
}

std::size_t FormatResolve::GetAlign() const {
    return _prevSpaceData.Align;
}

void FormatResolve::SetRelativeIndentAlign(std::size_t align) {
    _prevSpaceStrategy = PrevSpaceStrategy::AlignRelativeIndent;
    _prevSpaceData.Align = align;
}

void FormatResolve::SetTokenStrategy(TokenStrategy strategy) {
    _tokenStrategy = strategy;
}

void FormatResolve::SetPrevTokenStrategy(PrevTokenStrategy strategy) {
    _prevTokenStrategy = strategy;
}

void FormatResolve::SetNextTokenStrategy(NextTokenStrategy strategy) {
    _nextAddStrategy = strategy;
}

void FormatResolve::SetOriginRange(IndexRange range) {
    _tokenStrategy = TokenStrategy::OriginRange;
    _range = range;
}

IndexRange FormatResolve::GetOriginRange() const {
    return _range;
}
