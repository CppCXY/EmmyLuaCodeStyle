#pragma once

#include <cinttypes>
#include <string>

namespace spell {
struct WordRange {
    std::size_t Start;
    std::size_t Count;

    WordRange(std::size_t start, std::size_t count)
            : Start(start),
              Count(count) {
    }
};

struct Word {
    WordRange Range;
    std::string Item;

    Word(const WordRange &range, const std::string &item)
            : Range(range),
              Item(item) {
    }
};
}
