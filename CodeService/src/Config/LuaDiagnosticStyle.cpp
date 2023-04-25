#include "CodeService/Config/LuaDiagnosticStyle.h"

#define BOOL_OPTION(op)                             \
    if (auto n = root.GetValue(#op); n.IsBool()) { \
        op = n.AsBool();                            \
    }

void LuaDiagnosticStyle::ParseTree(InfoTree &tree) {
    auto root = tree.GetRoot();

    BOOL_OPTION(code_style_check);

    BOOL_OPTION(name_style_check);

    BOOL_OPTION(spell_check);

//    if (auto n = root.GetValue("local_name_style"); n.IsObject()) {
//        if (n.IsArray()) {
//            local_name_style.clear();
//            for (auto c: n.GetChildren()) {
//                if (c.IsNull())
//            }
//        }
//    }
}
