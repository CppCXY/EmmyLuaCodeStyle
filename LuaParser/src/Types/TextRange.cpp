#include "LuaParser/Types/TextRange.h"

TextRange::TextRange()
        : TextRange(0, 0) {

}


TextRange::TextRange(std::size_t startOffset, std::size_t endOffset)
        :
        StartOffset(startOffset),
        EndOffset(endOffset) {
}

bool TextRange::IsEmpty() const {
    return StartOffset == 0 && EndOffset == 0;
}

bool TextRange::Contain(TextRange &range) const {
    return this->StartOffset <= range.StartOffset && this->EndOffset >= range.EndOffset;
}

bool TextRange::ContainOffset(std::size_t offset) const {
    return this->StartOffset <= offset && offset <= this->EndOffset;
}

bool TextRange::Between(TextRange &leftRange, TextRange &rightRange) const {
    return this->StartOffset > leftRange.EndOffset && this->EndOffset < rightRange.StartOffset;
}

