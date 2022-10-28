#include "CodeService/Format/FormatBuilder.h"
#include "Analyzer/SpaceAnalyzer.h"
#include "Analyzer/IntentionAnalyzer.h"

FormatBuilder::FormatBuilder() {

}

void FormatBuilder::FormatAnalyze(const LuaAstTree &t) {
    LuaAstNode root(0);
    auto descendants = root.GetDescendants(t);
    SpaceAnalyzer a;
    a.Analyze(*this, descendants, t);

    IntentionAnalyzer b;
    b.Analyze(*this, descendants, t);


}
