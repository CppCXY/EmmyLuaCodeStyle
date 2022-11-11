#include "CodeService/Format/FormatBuilder.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"

FormatBuilder::FormatBuilder() {

}

void FormatBuilder::FormatAnalyze(const LuaSyntaxTree &t) {
    AddAnalyzer<SpaceAnalyzer>();
    AddAnalyzer<IndentationAnalyzer>();
    AddAnalyzer<LineBreakAnalyzer>();

    for (const auto &analyzer: _analyzers) {
        analyzer->Analyze(*this, t);
    }
}

std::string FormatBuilder::GetFormatResult(const LuaSyntaxTree &t) {
    enum class TraverseEvent {
        Enter,
        Exit
    };

    struct Traverse {
        Traverse(LuaSyntaxNode n, TraverseEvent e)
                : Node(n), Event(e) {}

        LuaSyntaxNode Node;
        TraverseEvent Event;
    };

    auto root = t.GetRootNode();
    std::vector<Traverse> traverseStack;
    traverseStack.emplace_back(root, TraverseEvent::Enter);
    // 非递归深度优先遍历
    while (!traverseStack.empty()) {
        Traverse traverse = traverseStack.back();

        if (traverse.Event == TraverseEvent::Enter) {
            traverseStack.back().Event = TraverseEvent::Exit;
        } else {
            traverseStack.pop_back();
            continue;
        }

        auto children = traverse.Node.GetChildren(t);
        // 不采用 <range>
        for (auto rIt = children.rbegin(); rIt != children.rend(); rIt++) {
            traverseStack.emplace_back(*rIt, TraverseEvent::Enter);
        }
        FormatResolve resolve;
        for (auto &analyzer: _analyzers) {
            analyzer->Query(*this, traverse.Node, t, resolve);
        }

        DoResolve(traverse.Node, t, resolve);
    }

    return _formattedText;
}

void FormatBuilder::WriteSyntaxNode(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t) {
    _formattedText.append(syntaxNode.GetText(t));
}

void FormatBuilder::DoResolve(LuaSyntaxNode &syntaxNode, const LuaSyntaxTree &t, FormatResolve &resolve) {
    if (syntaxNode.IsToken(t)) {
        switch (resolve.GetTokenStrategy()) {
            case TokenStrategy::Origin: {
                WriteSyntaxNode(syntaxNode, t);
                break;
            }
            case TokenStrategy::Remove: {
                break;
            }
            case TokenStrategy::StringSingleQuote: {
                if (syntaxNode.GetTokenKind(t) == TK_STRING) {

                }
                break;
            }
            default: {
                break;
            }
        }
    }
    switch (resolve.GetSpaceStrategy()) {
        case SpaceStrategy::Space: {
            WriteSpace(resolve.GetNextSpace());
            break;
        }
        case SpaceStrategy::LineBreak: {
            WriteLine(resolve.GetNextLine());
            break;
        }
        default: {
            break;
        }
    }
}

void FormatBuilder::WriteSpace(std::size_t space) {
    if (space == 0) {
        return;
    } else if (space == 1) {
        _formattedText.push_back(' ');
    } else {
        auto size = _formattedText.size();
        _formattedText.resize(size + space, ' ');
    }
}

void FormatBuilder::WriteLine(std::size_t line) {
    if (line == 0) {
        return;
    }
    auto endOfLine = EndOfLine::CRLF;
    switch (endOfLine) {
        case EndOfLine::CRLF: {
            if (line == 1) {
                _formattedText.push_back('\r');
                _formattedText.push_back('\n');
            } else {
                auto index = _formattedText.size();
                _formattedText.resize(2 * line, '\n');
                for (; index < _formattedText.size(); index += 2) {
                    _formattedText[index] = '\r';
                }
            }
            break;
        }
        case EndOfLine::CR: {
            if (line == 1) {
                _formattedText.push_back('\r');
            } else {
                _formattedText.resize(_formattedText.size() + line, '\r');
            }
            break;
        }
        case EndOfLine::UNKNOWN:
        case EndOfLine::MIX:
        case EndOfLine::LF: {
            if (line == 1) {
                _formattedText.push_back('\n');
            } else {
                _formattedText.resize(_formattedText.size() + line, '\n');
            }
            break;
        }
    }
}


