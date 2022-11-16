#pragma once

enum class FormatAnalyzerType {
    SpaceAnalyzer,
    LineBreakAnalyzer,
    IndentationAnalyzer,

    PluginAnalyzer
};

#define DECLARE_FORMAT_ANALYZER(CLASS) \
inline static constexpr FormatAnalyzerType Type = FormatAnalyzerType::CLASS;   \
FormatAnalyzerType GetType() const override { return Type; }
