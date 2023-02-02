#include "FunctionOption.h"
#include "LuaParser/Lexer/LuaLexer.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

void FunctionOption::Parse(std::string_view option) {
    std::string text(option);
    auto virtualFile = std::make_shared<LuaFile>(std::move(text));
    LuaLexer lexer(virtualFile);
    lexer.Parse();
    if (lexer.HasError()) {
        return;
    }
    enum class ParseState {
        None,
        Key,
        ExpectParam,
        ExpectCommaOrFinish
    } state = ParseState::None;
    auto &tokens = lexer.GetTokens();
    for (auto token: tokens) {
        switch (state) {
            case ParseState::None: {
                if (token.TokenType != TK_NAME) {
                    return;
                }
                _key = std::string(
                        virtualFile->Slice(token.Range.StartOffset, token.Range.EndOffset));
                state = ParseState::Key;
                break;
            }
            case ParseState::Key: {
                if (token.TokenType != '(') {
                    return;
                }
                state = ParseState::ExpectParam;
                break;
            }
            case ParseState::ExpectParam: {
                if (token.TokenType == TK_NAME || token.TokenType == TK_NUMBER || token.TokenType == TK_STRING) {
                    _params.emplace_back(virtualFile->Slice(token.Range.StartOffset, token.Range.EndOffset));
                    state = ParseState::ExpectCommaOrFinish;
                } else if (token.TokenType == ')') {
                    return;
                }
                break;
            }
            case ParseState::ExpectCommaOrFinish: {
                if (token.TokenType == ',') {
                    state = ParseState::ExpectParam;
                } else if (token.TokenType == ')') {
                    return;
                }
                break;
            }
        }
    }
}

std::string &FunctionOption::GetKey() {
    return _key;
}

std::string FunctionOption::GetParam(std::size_t index) {
    if (index < _params.size()) {
        return _params[index];
    }
    return "";
}

std::size_t FunctionOption::GetParamSize() {
    return _params.size();
}
