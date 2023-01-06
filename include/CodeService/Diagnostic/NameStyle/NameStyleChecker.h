//#pragma once
//
//#include <vector>
//#include <map>
//#include <string>
//#include "NameDefineType.h"
//#include "CheckElement.h"
//#include "CodeService/Diagnostic/DiagnosticBuilder.h"
//#include "LuaParser/Ast/LuaSyntaxTree.h"
//
//class NameStyleChecker {
//public:
//    class Scope {
//    public:
//        std::map<std::string, std::shared_ptr<CheckElement>, std::less<>> LocalVariableMap;
//    };
//
//    NameStyleChecker();
//
//    void Analyze(DiagnosticBuilder &d, const LuaSyntaxTree &t);
//
//private:
//    void CollectToken(DiagnosticBuilder &d, const LuaSyntaxTree &t);
//
//    void RecordLocalVariable(std::shared_ptr<CheckElement> checkElement);
//
//    bool IsGlobal(LuaSyntaxNode &n);
//
//    static std::set<std::string, std::less<>> TableFieldSpecialName;
//    static std::set<std::string, std::less<>> GlobalSpecialName;
//
//    // block到作用域的映射
//    std::map<std::size_t, Scope> _scopeMap;
//    std::vector<std::shared_ptr<CheckElement>> _nameStyleCheckVector;
//};
