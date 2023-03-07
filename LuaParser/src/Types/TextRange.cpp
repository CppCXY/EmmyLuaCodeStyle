#include "LuaParser/Types/TextRange.h"

TextRange::TextRange()
        : TextRange(0, 0) {

}

TextRange::TextRange(std::size_t startOffset, std::size_t length)
        : StartOffset(startOffset),
          Length(length) {

}

std::size_t TextRange::GetEndOffset() const {
    if (Length == 0) {
        return StartOffset;
    }
    return StartOffset + Length - 1;
}
