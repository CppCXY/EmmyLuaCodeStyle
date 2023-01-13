#include "CodeService/TypeFormat/LuaTypeFormat.h"
#include <algorithm>
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "Util/StringUtil.h"
#include "Util/format.h"

//std::shared_ptr<LuaAstNode> FindAstNodeBeforePosition(std::shared_ptr<LuaAstNode> root, int offset) {
//    if (root->GetChildren().empty()) {
//        return root;
//    }
//
//    enum class FindState {
//        None,
//        Before,
//        Contain
//    } state = FindState::None;
//
//    auto it = root->GetChildren().begin();
//    for (; it != root->GetChildren().end(); it++) {
//        auto &child = *it;
//        auto textRange = child->GetTextRange();
//        if (textRange.StartOffset > offset) {
//            state = FindState::Before;
//            break;
//        } else if (offset <= textRange.EndOffset) {
//            state = FindState::Contain;
//            break;
//        }
//    }
//
//    switch (state) {
//        case FindState::Before: {
//            if (it == root->GetChildren().begin()) {
//                return root;
//            }
//            return *(it - 1);
//        }
//        case FindState::Contain: {
//            return FindAstNodeBeforePosition(*it, offset);
//        }
//        case FindState::None: {
//            return root->GetChildren().back();
//        }
//    }
//    return nullptr;
//}


std::vector<LuaTypeFormat::Result> LuaTypeFormat::GetResult() {
    return _results;
}

//void LuaTypeFormat::FormatLine(int line) {
//    LuaCodeStyleOptions temp = _options;
//    temp.insert_final_newline = true;
//    temp.remove_expression_list_finish_comma = false;
//    if (_typeOptions.auto_complete_table_sep) {
//        temp.trailing_table_separator = TrailingTableSeparator::Smart;
//    } else {
//        temp.trailing_table_separator = TrailingTableSeparator::Keep;
//    }
//
//    LuaFormatter formatter(_parser, temp);
//    formatter.BuildFormattedElement();
//
//    auto &result = _results.emplace_back();
//    result.Range.StartLine = line - 1;
//    result.Range.StartCharacter = 0;
//    result.Range.EndLine = line - 1;
//    result.Range.EndCharacter = 0;
//    auto formatText = formatter.GetRangeFormattedText(result.Range);
//    while (!formatText.empty()) {
//        char ch = formatText.back();
//        if (ch == ' ') {
//            formatText.pop_back();
//        } else {
//            break;
//        }
//    }
//    if (!formatText.empty() && formatText.back() != '\n') {
//        formatText.push_back('\n');
//    }
//    result.Text = formatText;
//    result.Range.EndLine++;
//    _hasResult = true;
//}

//void LuaTypeFormat::FixIndent(int line, int character) {
//    // FixEndIndent(line, character);
//
//    // auto root = _parser->GetAst();
//    // auto astNode = FindAstNodeBeforePosition(root, offset);
//    // if (!astNode)
//    // {
//    // 	return;
//    // }
//
//    // auto block = astNode->GetParent();
//    // if (block && block->GetType() == LuaNodeType::Block)
//    // {
//    // 	auto statement = block->GetParent();
//    // 	if(statement->is)
//    //
//    //
//    // }
//
//
//}

//void LuaTypeFormat::FixEndIndent(int line, int character) {
//    auto luaFile = _parser->GetLuaFile();
//    auto offset = luaFile->GetOffsetFromPosition(line, character);
//
//    auto &tokens = _parser->GetLuaLexer()->GetTokens();
//    auto tokenIndex = FindTokenIndexAfterPosition(tokens, offset);
//    if (tokenIndex == -1) {
//        return;
//    }
//
//    auto token = tokens[tokenIndex];
//    if (token.TokenType == TK_END && luaFile->GetStartLine(token.Range.StartOffset) == line) {
//        _hasResult = true;
//        auto indentString = luaFile->GetIndentString(token.Range.StartOffset);
//        {
//            auto &result = _results.emplace_back();
//            SerializeContext ctx1(_parser, _options);
//            ctx1.Print(indentString);
//            ctx1.PrintIndent(4, _options.indent_style);
//            result.Text = ctx1.GetText();
//            result.Range.StartLine = line;
//            result.Range.StartCharacter = character;
//            result.Range.EndLine = line;
//            result.Range.EndCharacter = character;
//        }
//        {
//            SerializeContext ctx2(_parser, _options);
//            auto &result = _results.emplace_back();
//            ctx2.PrintLine(1);
//            ctx2.Print(indentString);
//            ctx2.Print("e");
//            result.Text = ctx2.GetText();
//            result.Range.StartLine = luaFile->GetStartLine(token.Range.StartOffset);
//            result.Range.StartCharacter = luaFile->GetColumn(token.Range.StartOffset);
//            result.Range.EndLine = result.Range.StartLine;
//            result.Range.EndCharacter = result.Range.StartCharacter + 1;
//        }
//    }
//}

LuaTypeFormat::LuaTypeFormat(LuaTypeFormatOptions &typeOptions)
        : _typeOptions(typeOptions),
          _hasResult(false) {

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
    if (line == 0) {
        return;
    }
    FormatRange formatRange;
    auto &file = t.GetFile();
    auto offset = file.GetOffset(line, character);
    auto prevToken = t.GetTokenBeforeOffset(offset);
    switch(prevToken.GetTokenKind(t)) {
        case TK_END: {
            auto parent = prevToken.GetParent(t);
            formatRange.StartLine = parent.GetStartLine(t);
            formatRange.EndLine = parent.GetEndLine(t);
            break;
        }
        default: {
            return;
        }
    }

    FormatBuilder f(style);
    f.FormatAnalyze(t);
    auto newText = f.GetRangeFormatResult(formatRange, t);
    auto &result = _results.emplace_back();
    result.Text = newText;
    result.Range = formatRange;
}

void LuaTypeFormat::FixIndent(std::size_t line, std::size_t character, const LuaSyntaxTree &t, LuaStyle &style) {

}

void LuaTypeFormat::FixEndIndent(std::size_t line, std::size_t character) {

}
