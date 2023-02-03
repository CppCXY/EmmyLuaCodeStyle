#pragma once

enum class FormatAnalyzerType {
    SpaceAnalyzer = 0,
    LineBreakAnalyzer,
    IndentationAnalyzer,
    AlignAnalyzer,
    TokenAnalyzer,

    Count,
};

#define DECLARE_FORMAT_ANALYZER(CLASS) \
inline static constexpr FormatAnalyzerType Type = FormatAnalyzerType::CLASS;   \
FormatAnalyzerType GetType() const override { return Type; }
