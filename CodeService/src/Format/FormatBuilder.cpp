#include "CodeService/Format/FormatBuilder.h"

#


FormatBuilder::FormatBuilder() {

}

void FormatBuilder::FormatAnalyze(const LuaSyntaxTree &t) {
    _spaceAnalyzer.Analyze(*this, t);
    _indentationAnalyzer.Analyze(*this, t);
    _lineBreakAnalyzer.Analyze(*this, t);
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
        }
        else{
            traverseStack.pop_back();
            continue;
        }

        auto children = traverse.Node.GetChildren(t);
        // 不采用 <range>
        for (auto rIt = children.rbegin(); rIt != children.rend(); rIt++) {
            traverseStack.emplace_back(*rIt, TraverseEvent::Enter);
        }

    }

    return _formattedText;
}

FormatResolve FormatBuilder::Resolve(LuaSyntaxNode syntaxNode, const LuaSyntaxTree &t) {
    return FormatResolve();
}

