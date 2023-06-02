#include "LuaCompile/Lua/LuaParser/LuaParser.h"
#include "LuaCompile/Lua/Define/LuaDefine.h"
#include "LuaCompile/Lua/Source/LuaSource.h"
//#include "LuaCompile/Lua/exception/LuaParseException.h"
#include <fmt/format.h>

using enum LuaTokenKind;

LuaParser::LuaParser(const LuaSource *source, std::vector<LuaToken> &&tokens)
    : _source(source),
      _tokens(tokens),
      _tokenIndex(0),
      _invalid(true),
      _current(LuaTokenKind::TK_EOF) {
}

bool LuaParser::Parse() {

    Body();

    if (_tokenIndex < _tokens.size()) {
        LuaError("parsing did not complete");
    }

    return true;
}

void LuaParser::Next() {
    auto tk = Current();
    auto me = MarkEvent(MarkEventType::EatToken);
    me.U.Token.Kind = tk;
    me.U.Token.Index = _tokenIndex;
    _p.GetEvents().push_back(me);
    _tokenIndex++;
    _invalid = true;
}

LuaTokenKind LuaParser::LookAhead() {
    std::size_t nextIndex = _tokenIndex + 1;

    while (nextIndex < _tokens.size()) {
        auto tk = _tokens[nextIndex].TokenType;
        switch (tk) {
            case TK_SHORT_COMMENT:
            case TK_LONG_COMMENT:
            case TK_SHEBANG:
            case TK_WS: {
                nextIndex++;
                break;
            }
            default: {
                return tk;
            }
        }
    }

    return TK_EOF;
}

LuaTokenKind LuaParser::Current() {
    if (_invalid) {
        _invalid = false;
        SkipTrivia();
        if (_tokenIndex < _tokens.size()) {
            _current = _tokens[_tokenIndex].TokenType;
        } else {
            _current = TK_EOF;
        }
    }

    return _current;
}

void LuaParser::SkipTrivia() {
    for (; _tokenIndex < _tokens.size(); _tokenIndex++) {
        switch (_tokens[_tokenIndex].TokenType) {
            case TK_WS:
            case TK_SHORT_COMMENT:
            case TK_LONG_COMMENT:
            case TK_SHEBANG: {
                // ignore
                break;
            }
            default: {
                return;
            }
        }
    }
}

bool LuaParser::BlockFollow(bool rightbrace) {
    switch (Current()) {
        case TK_ELSE:
        case TK_ELSEIF:
        case TK_END:
        case TK_EOF:
        case TK_UNTIL:
            return true;
        case TK_RPARN:
            return rightbrace;
        default:
            return false;
    }
}

void LuaParser::StatementList() {
    while (!BlockFollow(true)) {
        if (Statement().IsNone()) {
            ErrorStatement();
        }
    }
}

CompleteMarker LuaParser::Statement() {
    switch (Current()) {
        case TK_SEMI: {
            auto m = _p.Mark();
            Next();
            return m.Complete(_p, LuaSyntaxNodeKind::EmptyStatement);
        }
        case TK_IF: {
            return IfStatement();
        }
        case TK_WHILE: {
            return WhileStatement();
        }
        case TK_DO: {
            return DoStatement();
        }
        case TK_FOR: {
            return ForStatement();
        }
        case TK_REPEAT: {
            return RepeatStatement();
        }
        case TK_FUNCTION: {
            return FunctionStatement();
        }
        case TK_LOCAL: {
            if (LookAhead() == TK_FUNCTION) {
                return LocalFunctionStatement();
            } else {
                return LocalStatement();
            }
        }
        case TK_DBCOLON: {
            return LabelStatement();
        }
        case TK_RETURN: {
            return ReturnStatement();
        }
        case TK_BREAK: {
            return BreakStatement();
        }
        case TK_GOTO: {
            return GotoStatement();
        }
        default: {
            return OtherStatement();
        }
    }
}

CompleteMarker LuaParser::ErrorStatement() {
    auto m = _p.Mark();

    LuaError("the statement could not be parsed");

    Next();

    return m.Complete(_p, LuaSyntaxNodeKind::Error);
}

/* ifstat -> IF cond THEN block {ELSEIF cond THEN block} [ELSE block] END */
CompleteMarker LuaParser::IfStatement() {
    auto m = _p.Mark();

    TestThenBlock();
    while (Current() == TK_ELSEIF) {
        TestThenBlock();
    }
    if (TestAndNext(TK_ELSE)) {
        Body();
    }
    CheckAndNext(TK_END);

    return m.Complete(_p, LuaSyntaxNodeKind::IfStatement);
}

/* whilestat -> WHILE cond DO block END */
CompleteMarker LuaParser::WhileStatement() {
    auto m = _p.Mark();

    CheckAndNext(TK_WHILE);

    Expression();

    CheckAndNext(TK_DO);

    Body();

    CheckAndNext(TK_END);

    return m.Complete(_p, LuaSyntaxNodeKind::WhileStatement);
}

CompleteMarker LuaParser::DoStatement() {
    auto m = _p.Mark();

    CheckAndNext(TK_DO);

    Body();

    CheckAndNext(TK_END);

    return m.Complete(_p, LuaSyntaxNodeKind::DoStatement);
}

/* forstat -> FOR (fornum | forlist) END */
CompleteMarker LuaParser::ForStatement() {
    // forstatement 只有一个 for 的token 节点 加上 forNumber或者forList 节点
    auto m = _p.Mark();

    CheckAndNext(TK_FOR);

    Check(TK_NAME);

    switch (LookAhead()) {
        case TK_EQ: {
            ForNumber();
            return m.Complete(_p, LuaSyntaxNodeKind::ForNumberStatement);
        }
        case TK_COMMA:
        case TK_IN: {
            ForList();
            return m.Complete(_p, LuaSyntaxNodeKind::ForRangeStatement);
        }
        default: {
            LuaExpectedError("'=' or 'in' expected");
        }
    }

    return m.Complete(_p, LuaSyntaxNodeKind::ForNumberStatement);
}

CompleteMarker LuaParser::ForNumber() {
    auto m = _p.Mark();

    CheckName();

    CheckAndNext(TK_EQ);

    Expression();

    CheckAndNext(TK_COMMA);

    Expression();

    if (TestAndNext(TK_COMMA))// optional step
    {
        Expression();
    }

    ForBody();

    return m.Complete(_p);
}

/* forlist -> NAME {,NAME} IN explist forbody */
CompleteMarker LuaParser::ForList() {
    auto m = _p.Mark();

    NameDefList(false);

    CheckAndNext(TK_IN);

    ExpressionList();

    ForBody();

    return m.Complete(_p);
}

CompleteMarker LuaParser::ForBody() {
    auto m = _p.Mark();

    CheckAndNext(TK_DO);

    Body();

    CheckAndNext(TK_END);

    return m.Complete(_p);
}

/* repeatstat -> REPEAT block UNTIL cond */
CompleteMarker LuaParser::RepeatStatement() {
    auto m = _p.Mark();

    CheckAndNext(TK_REPEAT);

    Body();

    CheckAndNext(TK_UNTIL);

    Expression();

    return m.Complete(_p, LuaSyntaxNodeKind::RepeatStatement);
}

CompleteMarker LuaParser::FunctionStatement() {
    auto m = _p.Mark();

    CheckAndNext(TK_FUNCTION);

    FunctionName();

    FunctionBody();

    return m.Complete(_p, LuaSyntaxNodeKind::FunctionStatement);
}

CompleteMarker LuaParser::LocalFunctionStatement() {
    auto m = _p.Mark();

    CheckAndNext(TK_LOCAL);

    CheckAndNext(TK_FUNCTION);

    CheckName();

    FunctionBody();

    return m.Complete(_p, LuaSyntaxNodeKind::LocalFunctionStatement);
}

/* stat -> LOCAL ATTRIB NAME {',' ATTRIB NAME} ['=' explist] */
CompleteMarker LuaParser::LocalStatement() {
    auto m = _p.Mark();

    CheckAndNext(TK_LOCAL);

    NameDefList(true);

    if (TestAndNext(TK_EQ)) {
        ExpressionList();
    }
    // 如果有一个分号则加入到localstatement
    TestAndNext(TK_SEMI);

    return m.Complete(_p, LuaSyntaxNodeKind::LocalStatement);
}

CompleteMarker LuaParser::LabelStatement() {
    auto m = _p.Mark();

    CheckAndNext(TK_DBCOLON);

    CheckName();

    CheckAndNext(TK_DBCOLON);

    return m.Complete(_p, LuaSyntaxNodeKind::LabelStatement);
}

CompleteMarker LuaParser::ReturnStatement() {
    auto m = _p.Mark();

    CheckAndNext(TK_RETURN);

    if (!(BlockFollow() || Current() == TK_SEMI)) {
        ExpressionList();
    }

    TestAndNext(TK_SEMI);

    return m.Complete(_p, LuaSyntaxNodeKind::ReturnStatement);
}

CompleteMarker LuaParser::BreakStatement() {
    auto m = _p.Mark();

    CheckAndNext(TK_BREAK);

    TestAndNext(TK_SEMI);

    return m.Complete(_p, LuaSyntaxNodeKind::BreakStatement);
}

CompleteMarker LuaParser::GotoStatement() {
    auto m = _p.Mark();

    CheckAndNext(TK_GOTO);

    CheckName();

    TestAndNext(TK_SEMI);

    return m.Complete(_p, LuaSyntaxNodeKind::GotoStatement);
}

// exprStat -> call | assignment
// assignment -> varList '=' exprList
CompleteMarker LuaParser::OtherStatement() {
    auto m = _p.Mark();
    auto cm = SuffixedExpression();
    if (cm.IsNone()) {
        return cm;
    }

    if (Current() == TK_EQ || Current() == TK_COMMA) {
        while (TestAndNext(TK_COMMA)) {
            SuffixedExpression();
        }
        cm = m.Complete(_p, LuaSyntaxNodeKind::VarList);
        m = cm.Precede(_p);

        CheckAndNext(TK_EQ);

        ExpressionList();

        // 如果发现一个分号，会认为分号为该语句的结尾
        TestAndNext(TK_SEMI);
        return m.Complete(_p, LuaSyntaxNodeKind::AssignStatement);
    } else {
        TestAndNext(TK_SEMI);
        return m.Complete(_p, LuaSyntaxNodeKind::ExpressionStatement);
    }
}

/* test_then_block -> [IF | ELSEIF] cond THEN block */
void LuaParser::TestThenBlock() {
    if (Current() == TK_IF || Current() == TK_ELSEIF) {
        Next();
    }
    Expression();

    CheckAndNext(TK_THEN);

    Body();
}

CompleteMarker LuaParser::NameDefList(bool supportAttribute) {
    auto m = _p.Mark();

    do {
        if (NameDef(supportAttribute).IsNone()) {
            break;
        }
    } while (TestAndNext(TK_COMMA));

    return m.Complete(_p, LuaSyntaxNodeKind::NameDefList);
}

CompleteMarker LuaParser::NameDef(bool supportAttribute) {
    auto m = _p.Mark();

    CheckAndNext(TK_NAME);
    if (supportAttribute) {
        LocalAttribute();
    }

    return m.Complete(_p, LuaSyntaxNodeKind::NameDef);
}

CompleteMarker LuaParser::Body() {
    auto m = _p.Mark();

    StatementList();

    return m.Complete(_p, LuaSyntaxNodeKind::Body);
}

/* explist -> expr { ',' expr } */
CompleteMarker LuaParser::ExpressionList(LuaTokenKind stopToken) {
    auto m = _p.Mark();

    Expression();
    while (TestAndNext(TK_COMMA)) {
        if (Current() == stopToken) {
            break;
        }

        Expression();
    }

    return m.Complete(_p, LuaSyntaxNodeKind::ExpressionList);
}

CompleteMarker LuaParser::Expression() {
    return Subexpression(0);
}

/*
* subexpr -> (simpleexp | unop subexpr) { binop subexpr }
*/
CompleteMarker LuaParser::Subexpression(int limit) {
    CompleteMarker cm;
    UnOpr uop = detail::lua_opr::GetUnaryOperator(Current());
    if (uop != UnOpr::OPR_NOUNOPR) /* prefix (unary) operator? */
    {
        auto m = _p.Mark();
        Next();
        Subexpression(UNARY_PRIORITY);
        cm = m.Complete(_p, LuaSyntaxNodeKind::UnaryExpression);
    } else {
        cm = SimpleExpression();
    }

    auto op = detail::lua_opr::GetBinaryOperator(Current());
    /* expand while operators have priorities higher than 'limit' */
    while (op != BinOpr::OPR_NOBINOPR && priority[static_cast<int>(op)].left > limit) {
        auto m = cm.Precede(_p);

        Next();

        Subexpression(priority[static_cast<int>(op)].right);

        cm = m.Complete(_p, LuaSyntaxNodeKind::BinaryExpression);
        // next op
        op = detail::lua_opr::GetBinaryOperator(Current());
    }

    return cm;
}

/* simpleexp -> FLT | INT | STRING | NIL | TRUE | FALSE | ... |
				constructor | FUNCTION body | suffixedexp */
CompleteMarker LuaParser::SimpleExpression() {
    switch (Current()) {
        case TK_FLT:
        case TK_NUMBER:
        case TK_NIL:
        case TK_TRUE:
        case TK_FALSE:
        case TK_DOTS: {
            auto m = _p.Mark();
            Next();
            return m.Complete(_p, LuaSyntaxNodeKind::LiteralExpression);
        }
        case TK_LONG_STRING:
        case TK_STRING: {
            auto m = _p.Mark();
            Next();
            return m.Complete(_p, LuaSyntaxNodeKind::StringLiteralExpression);
        }
        case TK_LCURLY: {
            return TableConstructor();
        }
        case TK_FUNCTION: {
            auto m = _p.Mark();

            Next();
            FunctionBody();

            return m.Complete(_p, LuaSyntaxNodeKind::ClosureExpression);
        }
        default: {
            return SuffixedExpression();
        }
    }
}

/* constructor -> '{' [ field { sep field } [sep] ] '}'
   sep -> ',' | ';'
*/
CompleteMarker LuaParser::TableConstructor() {
    auto m = _p.Mark();
    CheckAndNext(TK_LCURLY);

    FieldList();

    CheckAndNext(TK_RCURLY);

    return m.Complete(_p, LuaSyntaxNodeKind::TableExpression);
}

CompleteMarker LuaParser::FieldList() {
    auto m = _p.Mark();

    while (Current() != TK_RCURLY) {
        Field();
        if (Current() == TK_EOF) {
            break;
        }
    }

    return m.Complete(_p, LuaSyntaxNodeKind::TableFieldList);
}

/* field -> listfield | recfield */
CompleteMarker LuaParser::Field() {
    auto m = _p.Mark();

    switch (Current()) {
        case TK_NAME: {
            if (LookAhead() != TK_EQ) {
                ListField();
            } else {
                RectField();
            }
            break;
        }
        case TK_LBRACKET: {
            RectField();
            break;
        }
        default: {
            ListField();
            break;
        }
    }

    if (Current() == TK_COMMA || Current() == TK_SEMI) {
        auto m2 = _p.Mark();
        Next();
        m2.Complete(_p, LuaSyntaxNodeKind::TableFieldSep);
    }

    return m.Complete(_p, LuaSyntaxNodeKind::TableField);
}

void LuaParser::ListField() {
    Expression();
}

/* recfield -> (NAME | '['exp']') = exp */
void LuaParser::RectField() {
    if (Current() == TK_NAME) {
        Next();
    } else {
        YIndex();
    }

    CheckAndNext(TK_EQ);

    Expression();
}

/* body ->  '(' parlist ')' block END */
CompleteMarker LuaParser::FunctionBody() {
    auto m = _p.Mark();

    CheckAndNext(TK_LPARN);

    ParamList();

    CheckAndNext(TK_RPARN);

    Body();

    CheckAndNext(TK_END);

    return m.Complete(_p, LuaSyntaxNodeKind::FunctionBody);
}

CompleteMarker LuaParser::ParamList() {
    auto m = _p.Mark();

    bool isVararg = false;
    if (Current() != TK_RPARN) {
        do {
            switch (Current()) {
                case TK_NAME: {
                    Next();
                    break;
                }
                case TK_DOTS: {
                    isVararg = true;
                    Next();
                    break;
                }
                case TK_RPARN: {
                    break;
                }
                default: {
                    LuaExpectedError("<name> or '...' expected");
                    goto endLoop;
                }
            }
        } while (!isVararg && TestAndNext(TK_COMMA));
    endLoop:
        // empty stat
        void(0);
    }

    return m.Complete(_p, LuaSyntaxNodeKind::ParamList);
}

/* suffixedexp -> primaryexp { '.' NAME | '[' exp ']' | ':' NAME funcargs | funcargs }+ */
/* primaryexp -> nameexp | parexp */
CompleteMarker LuaParser::SuffixedExpression() {
    auto m = _p.Mark();
    auto cm = PrimaryExpression();
    bool suffix = false;
    for (;;) {
        switch (Current()) {
            case TK_DOT: {
                FieldSel();
                break;
            }
            case TK_LBRACKET: {
                YIndex();
                break;
            }
            case TK_COLON: {
                FieldSel();
                CallExpression();
                break;
            }
            case TK_LPARN:
            case TK_LONG_STRING:
            case TK_STRING:
            case TK_LCURLY: {
                CallExpression();
                break;
            }
            default:
                goto endLoop;
        }
        suffix = true;
    }
endLoop:
    if (suffix) {
        return m.Complete(_p, LuaSyntaxNodeKind::SuffixedExpression);
    } else {
        m.Undo(_p);
        return cm;
    }
}

CompleteMarker LuaParser::CallExpression() {
    auto m = _p.Mark();

    switch (Current()) {
        case TK_LPARN: {
            Next();
            if (Current() != TK_RPARN) {
                // extend grammar, allow pcall(1,2,)
                ExpressionList(TK_RPARN);
            }

            CheckAndNext(TK_RPARN);
            break;
        }
        case TK_LCURLY: {
            TableConstructor();
            break;
        }
        case TK_LONG_STRING:
        case TK_STRING: {
            auto sm = _p.Mark();
            Next();
            sm.Complete(_p, LuaSyntaxNodeKind::StringLiteralExpression);
            break;
        }
        default: {
            LuaExpectedError("function arguments expected");
        }
    }

    return m.Complete(_p, LuaSyntaxNodeKind::CallExpression);
}

/* fieldsel -> ['.' | ':'] NAME */
CompleteMarker LuaParser::FieldSel() {
    auto m = _p.Mark();

    Next();

    CheckAndNext(TK_NAME);

    return m.Complete(_p, LuaSyntaxNodeKind::IndexExpression);
}

/* index -> '[' expr ']' */
CompleteMarker LuaParser::YIndex() {
    auto m = _p.Mark();

    CheckAndNext(TK_LBRACKET);

    Expression();

    CheckAndNext(TK_RBRACKET);

    return m.Complete(_p, LuaSyntaxNodeKind::IndexExpression);
}

void LuaParser::FunctionName() {
    auto m = _p.Mark();

    CheckAndNext(TK_NAME);

    while (Current() == TK_DOT) {
        FieldSel();
    }

    if (Current() == TK_COLON) {
        FieldSel();
    }

    m.Complete(_p, LuaSyntaxNodeKind::FunctionNameExpression);
}

void LuaParser::CheckName() {
    if (Current() == TK_NAME) {
        Next();
    }
}

/* ATTRIB -> ['<' Name '>'] */
CompleteMarker LuaParser::LocalAttribute() {
    auto m = _p.Mark();
    if (TestAndNext(TK_LT)) {
        if (Current() == TK_NAME) {
            auto range = _tokens[_tokenIndex].Range;
            auto attr = _source->Slice(range);
            if (attr != "const" && attr != "close") {
                LuaExpectedError(fmt::format("unknown attribute '{}'", attr));
            }
            Next();
        } else {
            LuaExpectedError(fmt::format("expected attribute 'const' or 'close'"));
        }

        CheckAndNext(TK_GT);
        return m.Complete(_p, LuaSyntaxNodeKind::Attribute);
    }
    return m.Undo(_p);
}

void LuaParser::Check(LuaTokenKind c) {
    if (Current() != c) {
        LuaExpectedError(fmt::format("'{}' expected", c));
    }
}

/* primaryexp -> NAME | '(' expr ')' */
CompleteMarker LuaParser::PrimaryExpression() {
    auto m = _p.Mark();
    switch (Current()) {
        case TK_LPARN: {
            Next();
            Expression();
            CheckAndNext(TK_RPARN);
            return m.Complete(_p, LuaSyntaxNodeKind::ParExpression);
        }
        case TK_NAME: {
            Next();
            return m.Complete(_p, LuaSyntaxNodeKind::NameExpression);
        }
        default:
            LuaExpectedError("unexpected symbol");
    }
    return m.Undo(_p);
}

void LuaParser::CheckAndNext(LuaTokenKind kind) {
    if (Current() != kind) {
        LuaExpectedError(fmt::format("token type {} expected", kind));
        return;
    }

    Next();
}

bool LuaParser::TestAndNext(LuaTokenKind kind) {
    if (Current() == kind) {
        Next();
        return true;
    }
    return false;
}

void LuaParser::LuaExpectedError(std::string_view message) {
    if (_tokenIndex < _tokens.size()) {
        _errors.emplace_back(message, _tokens[_tokenIndex].Range);
    } else if (!_tokens.empty()) {
        auto tokenIndex = _tokens.size() - 1;
        _errors.emplace_back(message, _tokens[tokenIndex].Range);
    } else {
        _errors.emplace_back(message, TextRange(0, 0));
    }
}

void LuaParser::LuaError(std::string_view message) {
    _errors.emplace_back(message, _tokens[_tokenIndex].Range);
}

ParseState &LuaParser::GetParseState() {
    return _p;
}

std::vector<LuaToken> &LuaParser::GetTokens() {
    return _tokens;
}

std::vector<LuaSyntaxError> &LuaParser::GetErrors() {
    return _errors;
}
