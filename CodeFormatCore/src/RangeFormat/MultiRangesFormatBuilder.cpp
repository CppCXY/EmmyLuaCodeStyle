#include "CodeFormatCore/RangeFormat/MultiRangesFormatBuilder.h"

MultiRangesFormatBuilder::MultiRangesFormatBuilder(LuaStyle &style, std::vector<FormatRange> &range) {
}

std::string MultiRangesFormatBuilder::GetMultiFormatResult(const LuaSyntaxTree &t) {
    return std::string();
}

void MultiRangesFormatBuilder::WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    FormatBuilder::WriteSyntaxNode(syntaxNode, t);
}

void MultiRangesFormatBuilder::WriteSpace(std::size_t space) {
    FormatBuilder::WriteSpace(space);
}

void MultiRangesFormatBuilder::WriteLine(std::size_t line) {
    FormatBuilder::WriteLine(line);
}

void MultiRangesFormatBuilder::WriteIndent() {
    FormatBuilder::WriteIndent();
}

void MultiRangesFormatBuilder::WriteChar(char ch) {
    FormatBuilder::WriteChar(ch);
}

void MultiRangesFormatBuilder::WriteText(std::string_view text) {
    FormatBuilder::WriteText(text);
}

void MultiRangesFormatBuilder::CheckRange(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
}
