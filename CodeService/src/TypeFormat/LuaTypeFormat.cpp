#include "CodeService/TypeFormat/LuaTypeFormat.h"
#include <algorithm>
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "Util/StringUtil.h"
#include "Util/format.h"
#include "CodeService/RangeFormat/RangeFormatBuilder.h"

std::vector<LuaTypeFormat::Result> LuaTypeFormat::GetResult() {
    return _results;
}

LuaTypeFormat::LuaTypeFormat(LuaTypeFormatOptions &typeOptions)
        : _typeOptions(typeOptions) {

}

void LuaTypeFormat::Analyze(std::string_view trigger,
                            std::size_t line,
                            std::size_t character,
                            const LuaSyntaxTree &t,
                            LuaStyle &style) {
    if (trigger == "\n") {
        return AnalyzeReturn(line, character, t, style);
    }
}

void LuaTypeFormat::AnalyzeReturn(std::size_t line, std::size_t character, const LuaSyntaxTree &t, LuaStyle &style) {
    if (line == 0) {
        return;
    }

    if (t.HasError()) {
        auto &errors = t.GetErrors();
        if (!errors.empty()) {
            if (errors.front().ExpectToken != 0) {
                return CompleteMissToken(line, character, errors.front(), t, style);
            }
        }
        return;
    }

    auto &luaFile = t.GetFile();
    if (luaFile.IsEmptyLine(line - 1)) {
        return;
    }

    if (_typeOptions.format_line) {
        FormatLine(line, character, t, style);
    }

    if (_typeOptions.fix_indent) {
        FixIndent(line, character, t, style);
    }
}

void LuaTypeFormat::CompleteMissToken(std::size_t line,
                                      std::size_t character,
                                      const LuaParseError &luaError,
                                      const LuaSyntaxTree &t,
                                      LuaStyle &style) {
    if (!_typeOptions.auto_complete_end) {
        return;
    }
    auto &result = _results.emplace_back();

    switch (luaError.ExpectToken) {
        case TK_END: {
            auto &file = t.GetFile();
            auto offset = file.GetOffset(line, character);
            auto beforeToken = t.GetTokenBeforeOffset(offset);
            if (beforeToken.IsNull(t)) {
                return;
            }

            auto keyOffset = beforeToken.GetTextRange(t).StartOffset;
            switch (beforeToken.GetTokenKind(t)) {
                case TK_DO:
                case TK_THEN:
                case TK_ELSE: {
                    break;
                }
                case ')': {
                    auto functionBody = beforeToken.GetParent(t);
                    if (functionBody.GetSyntaxKind(t) == LuaSyntaxNodeKind::FunctionBody) {
                        keyOffset = functionBody.GetTextRange(t).StartOffset;
                    }
                    break;
                }
                default: {
                    return;
                }
            }

            bool nextBrace = false;
            auto indentString = file.GetIndentString(keyOffset);
            auto nextToken = beforeToken.GetNextToken(t);
            if (nextToken.GetTokenKind(t) == TK_END) {
                // 依据缩进判断是否该填补end
                auto endIndentString = file.GetIndentString(nextToken.GetTextRange(t).StartOffset);
                if (indentString == endIndentString) {
                    return;
                }
            } else {
                auto nextTokenKind = nextToken.GetTokenKind(t);
                switch (nextTokenKind) {
                    case ')':
                    case ',': {
                        nextBrace = true;
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }

            if (nextBrace) {
                result.Text = util::format("end{}", nextToken.GetText(t));
                result.Range.StartLine = nextToken.GetStartLine(t);
                result.Range.StartCol = nextToken.GetStartCol(t);
                result.Range.EndLine = nextToken.GetEndLine(t);
                result.Range.EndCol = nextToken.GetEndCol(t) + 1;
            } else {
                auto fileEndOfLine = file.GetEndOfLine();
                std::string endOfLine = "\r\n";
                switch (style.detect_end_of_line ? fileEndOfLine : style.end_of_line) {
                    case EndOfLine::CRLF: {
                        endOfLine = "\r\n";
                        break;
                    }
                    case EndOfLine::CR: {
                        endOfLine = '\r';
                        break;
                    }
                    case EndOfLine::MIX:
                    case EndOfLine::UNKNOWN:
                    case EndOfLine::LF: {
                        endOfLine = '\n';
                        break;
                    }
                }

                result.Text = util::format("{}end{}", indentString, endOfLine);
                auto totalLine = file.GetTotalLine();
                if (line >= totalLine) {
                    result.Text = endOfLine + result.Text;
                }

                result.Range.StartLine = line + 1;
                result.Range.StartCol = 0;
                result.Range.EndLine = line + 1;
                result.Range.EndCol = 0;
            }

            break;
        }
        default: {
            break;
        }
    }
}

void LuaTypeFormat::FormatLine(std::size_t line, std::size_t character, const LuaSyntaxTree &t, LuaStyle &style) {
    FormatRange formatRange;
    auto &file = t.GetFile();
    auto offset = file.GetOffset(line, character);
    auto prevToken = t.GetTokenBeforeOffset(offset);
    auto tempStyle = style;
    switch (prevToken.GetTokenKind(t)) {
        case TK_END: {
            auto parent = prevToken.GetParent(t);
            formatRange.StartLine = parent.GetStartLine(t);
            formatRange.EndLine = parent.GetEndLine(t);
            break;
        }
        default: {
            formatRange.StartLine = line - 1;
            formatRange.EndLine = line - 1;

            if (_typeOptions.auto_complete_table_sep) {
                tempStyle.trailing_table_separator = TrailingTableSeparator::Smart;
            }

            break;
        }
    }
    tempStyle.call_arg_parentheses = CallArgParentheses::Keep;
    return FormatByRange(formatRange, t, tempStyle);
}

void LuaTypeFormat::FixIndent(std::size_t line, std::size_t character, const LuaSyntaxTree &t, LuaStyle &style) {

}

void LuaTypeFormat::FixEndIndent(std::size_t line, std::size_t character) {

}

void LuaTypeFormat::FormatByRange(FormatRange range, const LuaSyntaxTree &t, LuaStyle &style) {
    RangeFormatBuilder f(style, range);
    auto newText = f.GetFormatResult(t);
    range = f.GetReplaceRange();
    auto &result = _results.emplace_back();
    result.Text = newText;
    range.EndLine++;
    result.Range = range;
}
