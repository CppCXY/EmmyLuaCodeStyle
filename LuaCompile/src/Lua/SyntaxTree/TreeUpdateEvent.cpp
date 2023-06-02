#include "LuaCompile/Lua/SyntaxTree/TreeUpdateEvent.h"
#include "LuaCompile/Lua/Kind/LuaTokenKind.h"
#include "LuaCompile/Lua/SyntaxTree/LuaNodeOrToken.h"

TreeUpdateEvent TreeUpdateEvent::From(SourceUpdateEvent &sourceUpdateEvent, LuaSyntaxTree &t) {
    TreeUpdateEvent e;
    e.SourceEvent = sourceUpdateEvent;
    switch (sourceUpdateEvent.UpdateAction) {
        case SourceUpdateEvent::Action::Add: {
            e.InitByAdd(sourceUpdateEvent, t);
            break;
        }
        case SourceUpdateEvent::Action::Delete: {
            e.InitByDelete(sourceUpdateEvent, t);
            break;
        }
        case SourceUpdateEvent::Action::Replace: {
            e.InitByReplace(sourceUpdateEvent, t);
            break;
        }
        case SourceUpdateEvent::Action::None: {
            break;
        }
    }

    return e;
}

TreeUpdateEvent::TreeUpdateEvent()
    : UpdateAction(Action::UpdateTree) {
}

bool TreeUpdateEvent::ContainSpace(std::string_view source) {
    for (auto c: source) {
        if (c > 0 && std::isspace(c)) {
            return true;
        }
    }
    return false;
}

bool TreeUpdateEvent::OnlySpace(std::string_view source) {
    for (auto c: source) {
        if (c < 0 || !std::isspace(c)) {
            return false;
        }
    }
    return true;
}

void TreeUpdateEvent::InitByAdd(SourceUpdateEvent &sourceUpdateEvent, LuaSyntaxTree &t) {
    auto token = t.GetTokenAtOffset(sourceUpdateEvent.Range.StartOffset);
    switch (token.GetTokenKind(t)) {
        case LuaTokenKind::TK_NAME: {
            //            if (!ContainSpace(sourceUpdateEvent.Text)) {
            //                auto originRange = token.GetTextRange(t);
            //                auto start = sourceUpdateEvent.Range.StartOffset - originRange.StartOffset;
            //                std::string tokenText(token.GetText(t));
            //                tokenText.insert(start, sourceUpdateEvent.Text);
            //                if (!LuaLexer::IsReserved(tokenText)) {
            //                    UpdateAction = Action::OnlyUpdateTokenOffset;
            //                }
            //            }
            break;
        }
        case LuaTokenKind::TK_WS: {
            if (OnlySpace(sourceUpdateEvent.Text)) {
                UpdateAction = Action::OnlyUpdateToken;
            }
            break;
        }
        default: {
            break;
        }
    }
}

void TreeUpdateEvent::InitByDelete(SourceUpdateEvent &sourceUpdateEvent, LuaSyntaxTree &t) {
    auto token = t.GetTokenAtOffset(sourceUpdateEvent.Range.StartOffset);
    switch (token.GetTokenKind(t)) {
        case LuaTokenKind::TK_WS: {
            auto originRange = token.GetTextRange(t);
            auto start = sourceUpdateEvent.Range.StartOffset - originRange.StartOffset;
            if (start == 0 && sourceUpdateEvent.Range.Length == originRange.Length) {
                return;
            }

            if (start + sourceUpdateEvent.Range.Length <= originRange.Length) {
                UpdateAction = Action::OnlyUpdateToken;
            }

            break;
        }
        default: {
            break;
        }
    }
}

void TreeUpdateEvent::InitByReplace(SourceUpdateEvent &sourceUpdateEvent, LuaSyntaxTree &t) {
    auto token = t.GetTokenAtOffset(sourceUpdateEvent.Range.StartOffset);
    switch (token.GetTokenKind(t)) {
        case LuaTokenKind::TK_WS: {
            if (!OnlySpace(sourceUpdateEvent.Text)) {
                return;
            }
            auto originRange = token.GetTextRange(t);
            auto start = sourceUpdateEvent.Range.StartOffset - originRange.StartOffset;
            if (start == 0 && sourceUpdateEvent.Range.Length == originRange.Length) {
                return;
            }

            if (start + sourceUpdateEvent.Range.Length <= originRange.Length) {
                UpdateAction = Action::OnlyUpdateToken;
            }

            break;
        }
        default: {
            break;
        }
    }
}
