#include "Util/Utf8.h"

std::size_t utf8::Utf8nLen(const char *source, std::size_t byteNum) {
    const char *t = source;
    std::size_t length = 0;

    while (static_cast<std::size_t>(source - t) < byteNum && '\0' != *source) {
        if (0xf0 == (0xf8 & *source)) {
            // 4-byte utf8 code point (began with 0b11110xxx)
            source += 4;
        } else if (0xe0 == (0xf0 & *source)) {
            // 3-byte utf8 code point (began with 0b1110xxxx)
            source += 3;
        } else if (0xc0 == (0xe0 & *source)) {
            // 2-byte utf8 code point (began with 0b110xxxxx)
            source += 2;
        } else {
            // if (0x00 == (0x80 & *s)) {
            // 1-byte ascii (began with 0b0xxxxxxx)
            source += 1;
        }

        // no matter the bytes we marched s forward by, it was
        // only 1 utf8 codepoint
        length++;
    }

    if (static_cast<size_t>(source - t) > byteNum) {
        length--;
    }
    return length;
}

std::size_t utf8::Utf8nByteNum(const char *source, std::size_t maxByteNum, std::size_t utf8Position) {
    const char *t = source;
    std::size_t length = 0;
    std::size_t byteNum = 0;
    while (byteNum < maxByteNum && '\0' != *source) {
        if (0xf0 == (0xf8 & *source)) {
            // 4-byte utf8 code point (began with 0b11110xxx)
            source += 4;
        } else if (0xe0 == (0xf0 & *source)) {
            // 3-byte utf8 code point (began with 0b1110xxxx)
            source += 3;
        } else if (0xc0 == (0xe0 & *source)) {
            // 2-byte utf8 code point (began with 0b110xxxxx)
            source += 2;
        } else {
            // if (0x00 == (0x80 & *s)) {
            // 1-byte ascii (began with 0b0xxxxxxx)
            source += 1;
        }

        // no matter the bytes we marched s forward by, it was
        // only 1 utf8 codepoint
        length++;

        if (length > utf8Position) {
            return byteNum;
        }

        byteNum = static_cast<std::size_t>(source - t);
    }

    if (byteNum > maxByteNum) {
        return maxByteNum;
    }

    return byteNum;
}

std::size_t utf8::Utf8nLenAtFirstLine(const char *source, std::size_t byteNum) {
    const char *t = source;
    std::size_t length = 0;

    while (static_cast<std::size_t>(source - t) < byteNum
           && (*source != '\0' && *source != '\r' && *source != '\n')) {
        if (0xf0 == (0xf8 & *source)) {
            // 4-byte utf8 code point (began with 0b11110xxx)
            source += 4;
        } else if (0xe0 == (0xf0 & *source)) {
            // 3-byte utf8 code point (began with 0b1110xxxx)
            source += 3;
        } else if (0xc0 == (0xe0 & *source)) {
            // 2-byte utf8 code point (began with 0b110xxxxx)
            source += 2;
        } else {
            // if (0x00 == (0x80 & *s)) {
            // 1-byte ascii (began with 0b0xxxxxxx)
            source += 1;
        }

        // no matter the bytes we marched s forward by, it was
        // only 1 utf8 codepoint
        length++;
    }

    if (static_cast<size_t>(source - t) > byteNum) {
        length--;
    }
    return length;
}

std::size_t utf8::Utf8OneCharLen(const char *source) {
    if (0xf0 == (0xf8 & *source)) {
        // 4-byte utf8 code point (began with 0b11110xxx)
        return 4;
    } else if (0xe0 == (0xf0 & *source)) {
        // 3-byte utf8 code point (began with 0b1110xxxx)
        return 3;
    } else if (0xc0 == (0xe0 & *source)) {
        // 2-byte utf8 code point (began with 0b110xxxxx)
        return 2;
    } else {
        // if (0x00 == (0x80 & *s)) {
        // 1-byte ascii (began with 0b0xxxxxxx)
        return 1;
    }
}
