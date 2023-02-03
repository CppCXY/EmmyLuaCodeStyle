#include "LuaParser/Parse/LuaParser.h"
#include "LuaParser/Lexer/LuaDefine.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "LuaParser/Parse/LuaOperatorType.h"
#include "Util/format.h"
#include "LuaParser/exception/LuaParseException.h"
#include "LuaParser/File/LuaFile.h"

LuaParser::LuaParser(std::shared_ptr<LuaFile> luaFile, std::vector<LuaToken> &&tokens)
        :
        _tokens(tokens),
        _tokenIndex(0),
        _file(luaFile),
        _events(),
        _invalid(true),
        _current(TK_EOF) {
}

std::vector<MarkEvent> &LuaParser::GetEvents() {
    return _events;
}

std::vector<LuaParseError> &LuaParser::GetErrors() {
    return _errors;
}

bool LuaParser::HasError() const {
    return !_errors.empty();
}

std::shared_ptr<LuaFile> LuaParser::GetLuaFile() {
    return _file;
}

bool LuaParser::Parse() {
    try {
        Block();
    }
    catch (LuaParseException &e) {
        auto text = _file->GetSource();
        _errors.emplace_back(e.what(), TextRange(text.size(), text.size()));
    }

    if (_tokenIndex < _tokens.size()) {
        _errors.emplace_back("parsing did not complete",
                             _tokens[_tokenIndex].Range);
    }

    return true;
}

Marker LuaParser::Mark() {
    auto pos = _events.size();
    _events.emplace_back();
    return Marker(pos);
}

void LuaParser::Next() {
    auto tk = Current();
    auto me = MarkEvent(MarkEventType::EatToken);
    me.U.Token.Kind = tk;
    me.U.Token.Index = _tokenIndex;
    _events.push_back(me);
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
            case TK_SHEBANG: {
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
        SkipComment();
        if (_tokenIndex < _tokens.size()) {
            _current = _tokens[_tokenIndex].TokenType;
        } else {
            _current = TK_EOF;
        }
    }

    if (_errors.size() > 100) {
        std::string_view error = "too many errors, parse fail";
        throw LuaParseException(error);
    }

    return _current;
}

void LuaParser::SkipComment() {
    for (; _tokenIndex < _tokens.size(); _tokenIndex++) {
        switch (_tokens[_tokenIndex].TokenType) {
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
        case ')':
            return rightbrace;
        default:
            return false;
    }
}

void LuaParser::StatementList() {
    while (!BlockFollow(true)) {
        Statement();
    }
}

void LuaParser::Statement() {
    switch (Current()) {
        case ';': {
            auto m = Mark();
            Next();
            m.Complete(*this, LuaSyntaxNodeKind::EmptyStatement);
            break;
        }
        case TK_IF: {
            IfStatement();
            break;
        }
        case TK_WHILE: {
            WhileStatement();
            break;
        }
        case TK_DO: {
            DoStatement();
            break;
        }
        case TK_FOR: {
            ForStatement();
            break;
        }
        case TK_REPEAT: {
            RepeatStatement();
            break;
        }
        case TK_FUNCTION: {
            FunctionStatement();
            break;
        }
        case TK_LOCAL: {
            if (LookAhead() == TK_FUNCTION) {
                LocalFunctionStatement();
            } else {
                LocalStatement();
            }
            break;
        }
        case TK_DBCOLON: {
            LabelStatement();
            break;
        }
        case TK_RETURN: {
            ReturnStatement();
            break;
        }
        case TK_BREAK: {
            BreakStatement();
            break;
        }
        case TK_GOTO: {
            GotoStatement();
            break;
        }
        default: {
            ExpressionStatement();
            break;
        }
    }
}

/* ifstat -> IF cond THEN block {ELSEIF cond THEN block} [ELSE block] END */
void LuaParser::IfStatement() {
    auto m = Mark();

    TestThenBlock();
    while (Current() == TK_ELSEIF) {
        TestThenBlock();
    }
    if (TestAndNext(TK_ELSE)) {
        Block();
    }
    CheckAndNext(TK_END);

    m.Complete(*this, LuaSyntaxNodeKind::IfStatement);
}

/* whilestat -> WHILE cond DO block END */
void LuaParser::WhileStatement() {
    auto m = Mark();

    CheckAndNext(TK_WHILE);

    Condition();

    CheckAndNext(TK_DO);

    Block();

    CheckAndNext(TK_END);

    m.Complete(*this, LuaSyntaxNodeKind::WhileStatement);
}

void LuaParser::DoStatement() {
    auto m = Mark();

    CheckAndNext(TK_DO);

    Block();

    CheckAndNext(TK_END);

    m.Complete(*this, LuaSyntaxNodeKind::DoStatement);
}

/* forstat -> FOR (fornum | forlist) END */
void LuaParser::ForStatement() {
    // forstatement 只有一个 for 的token 节点 加上 forNumber或者forList 节点
    auto m = Mark();

    CheckAndNext(TK_FOR);

    Check(TK_NAME);

    switch (LookAhead()) {
        case '=': {
            ForNumber();
            break;
        }
        case ',':
        case TK_IN: {
            ForList();
            break;
        }
        default: {
            LuaExpectedError("'=' or 'in' expected");
        }
    }

    m.Complete(*this, LuaSyntaxNodeKind::ForStatement);
}

void LuaParser::ForNumber() {
    auto m = Mark();

    CheckName();

    CheckAndNext('=');

    Expression();

    CheckAndNext(',');

    Expression();

    if (TestAndNext(',')) // optional step
    {
        Expression();
    }

    ForBody();

    m.Complete(*this, LuaSyntaxNodeKind::ForNumber);
}

/* forlist -> NAME {,NAME} IN explist forbody */
void LuaParser::ForList() {
    auto m = Mark();

    NameDefList();

    CheckAndNext(TK_IN);

    ExpressionList();

    ForBody();

    m.Complete(*this, LuaSyntaxNodeKind::ForList);
}

void LuaParser::ForBody() {
    auto m = Mark();

    CheckAndNext(TK_DO);

    Block();

    CheckAndNext(TK_END);

    m.Complete(*this, LuaSyntaxNodeKind::ForBody);
}

/* repeatstat -> REPEAT block UNTIL cond */
void LuaParser::RepeatStatement() {
    auto m = Mark();

    CheckAndNext(TK_REPEAT);

    Block();

    CheckAndNext(TK_UNTIL);

    Condition();

    m.Complete(*this, LuaSyntaxNodeKind::RepeatStatement);
}

void LuaParser::FunctionStatement() {
    auto m = Mark();

    CheckAndNext(TK_FUNCTION);

    FunctionName();

    FunctionBody();

    m.Complete(*this, LuaSyntaxNodeKind::FunctionStatement);
}

void LuaParser::LocalFunctionStatement() {
    auto m = Mark();

    CheckAndNext(TK_LOCAL);

    CheckAndNext(TK_FUNCTION);

    CheckName();

    FunctionBody();

    m.Complete(*this, LuaSyntaxNodeKind::LocalFunctionStatement);
}

/* stat -> LOCAL ATTRIB NAME {',' ATTRIB NAME} ['=' explist] */
void LuaParser::LocalStatement() {
    auto m = Mark();

    CheckAndNext(TK_LOCAL);

    auto nm = Mark();
    do {
        CheckName();
        LocalAttribute();
    } while (TestAndNext(','));
    nm.Complete(*this, LuaSyntaxNodeKind::NameDefList);

    if (TestAndNext('=')) {
        ExpressionList();
    }
    // 如果有一个分号则加入到localstatement
    TestAndNext(';');

    m.Complete(*this, LuaSyntaxNodeKind::LocalStatement);
}

void LuaParser::LabelStatement() {
    auto m = Mark();

    CheckAndNext(TK_DBCOLON);

    CheckName();

    CheckAndNext(TK_DBCOLON);

    m.Complete(*this, LuaSyntaxNodeKind::LabelStatement);
}

void LuaParser::ReturnStatement() {
    auto m = Mark();

    CheckAndNext(TK_RETURN);

    if (!(BlockFollow() || Current() == ';')) {
        ExpressionList();
    }

    TestAndNext(';');

    m.Complete(*this, LuaSyntaxNodeKind::ReturnStatement);
}

void LuaParser::BreakStatement() {
    auto m = Mark();

    CheckAndNext(TK_BREAK);

    TestAndNext(';');

    m.Complete(*this, LuaSyntaxNodeKind::BreakStatement);
}

void LuaParser::GotoStatement() {
    auto m = Mark();

    CheckAndNext(TK_GOTO);

    CheckName();

    TestAndNext(';');

    m.Complete(*this, LuaSyntaxNodeKind::GotoStatement);
}

// exprStat -> call | assignment
// assignment -> varList '=' exprList
void LuaParser::ExpressionStatement() {
    auto m = Mark();
    SuffixedExpression();
    if (Current() == '=' || Current() == ',') {
        while (TestAndNext(',')) {
            SuffixedExpression();
        }
        auto cm = m.Complete(*this, LuaSyntaxNodeKind::VarList);
        m = cm.Precede(*this);

        CheckAndNext('=');

        ExpressionList();

        // 如果发现一个分号，会认为分号为该语句的结尾
        TestAndNext(';');
        m.Complete(*this, LuaSyntaxNodeKind::AssignStatement);
    } else {
        TestAndNext(';');
        m.Complete(*this, LuaSyntaxNodeKind::ExpressionStatement);
    }
}

//
//void LuaParser::DocStatement(std::shared_ptr<LuaSyntaxNode> comment, LuaToken &docComment) {
//    LuaDocTokenParser docTokenParser(docComment.Text, docComment.Range);
//    docTokenParser.Next();
//
//    switch (docTokenParser.Current().TokenType) {
//        case TK_DOC_TAG_FORMAT: {
//            DocTagFormatStatement(comment, docTokenParser);
//            break;
//        }
//        default: {
//            break;
//        }
//    }
//}
//
//void LuaParser::DocTagFormatStatement(std::shared_ptr<LuaSyntaxNode> comment, LuaDocTokenParser &docTokenParser) {
//    auto docFormat = CreateAstNodeFromToken(LuaSyntaxNodeKind::DocTagFormat, docTokenParser.Current());
//
//    docTokenParser.Next();
//    switch (docTokenParser.Current().TokenType) {
//        case TK_DOC_DISABLE:
//        case TK_DOC_DISABLE_NEXT: {
//            docFormat->AddChild(CreateAstNodeFromToken(LuaSyntaxNodeKind::KeyWord, docTokenParser.Current()));
//            break;
//        }
//        default: {
//            return;
//        }
//    }
//
//    comment->AddChild(docFormat);
//}

void LuaParser::Condition() {
    Expression();
}

/* test_then_block -> [IF | ELSEIF] cond THEN block */
void LuaParser::TestThenBlock() {
    if (Current() == TK_IF || Current() == TK_ELSEIF) {
        Next();
    }
    Expression();
    CheckAndNext(TK_THEN);
    Block();
}

void LuaParser::Block() {
    auto m = Mark();

    StatementList();

    m.Complete(*this, LuaSyntaxNodeKind::Block);
}

/* explist -> expr { ',' expr } */
void LuaParser::ExpressionList(LuaTokenKind stopToken) {
    auto m = Mark();

    Expression();
    while (TestAndNext(',')) {
        if (Current() == stopToken) {
            break;
        }

        Expression();
    }

    m.Complete(*this, LuaSyntaxNodeKind::ExpressionList);
}

void LuaParser::Expression() {
    Subexpression(0);
}

/*
* subexpr -> (simpleexp | unop subexpr) { binop subexpr }
*/
void LuaParser::Subexpression(int limit) {
    CompleteMarker cm;
    UnOpr uop = GetUnaryOperator(Current());
    if (uop != UnOpr::OPR_NOUNOPR) /* prefix (unary) operator? */
    {
        auto m = Mark();
        Next();
        Subexpression(UNARY_PRIORITY);
        cm = m.Complete(*this, LuaSyntaxNodeKind::UnaryExpression);
    } else {
        cm = SimpleExpression();
    }

    auto op = GetBinaryOperator(Current());
    /* expand while operators have priorities higher than 'limit' */
    while (op != BinOpr::OPR_NOBINOPR && priority[static_cast<int>(op)].left > limit) {
        auto m = cm.Precede(*this);

        Next();

        Subexpression(priority[static_cast<int>(op)].right);

        cm = m.Complete(*this, LuaSyntaxNodeKind::BinaryExpression);
        // next op
        op = GetBinaryOperator(Current());
    }
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
            auto m = Mark();
            Next();
            return m.Complete(*this, LuaSyntaxNodeKind::LiteralExpression);
        }
        case TK_LONG_STRING:
        case TK_STRING: {
            auto m = Mark();
            Next();
            return m.Complete(*this, LuaSyntaxNodeKind::StringLiteralExpression);
        }
        case '{': {
            return TableConstructor();
        }
        case TK_FUNCTION: {
            auto m = Mark();

            Next();
            FunctionBody();

            return m.Complete(*this, LuaSyntaxNodeKind::ClosureExpression);
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
    auto m = Mark();
    CheckAndNext('{');

    FieldList();

    CheckAndNext('}');

    return m.Complete(*this, LuaSyntaxNodeKind::TableExpression);
}

void LuaParser::FieldList() {
    auto m = Mark();

    while (Current() != '}') {
        Field();
        if (Current() == TK_EOF) {
            break;
        }
    }

    m.Complete(*this, LuaSyntaxNodeKind::TableFieldList);
}

/* field -> listfield | recfield */
void LuaParser::Field() {
    auto m = Mark();

    switch (Current()) {
        case TK_NAME: {
            if (LookAhead() != '=') {
                ListField();
            } else {
                RectField();
            }
            break;
        }
        case '[': {
            RectField();
            break;
        }
        default: {
            ListField();
            break;
        }
    }

    if (Current() == ',' || Current() == ';') {
        auto m2 = Mark();
        Next();
        m2.Complete(*this, LuaSyntaxNodeKind::TableFieldSep);
    }

    m.Complete(*this, LuaSyntaxNodeKind::TableField);
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

    CheckAndNext('=');

    Expression();
}

/* body ->  '(' parlist ')' block END */
void LuaParser::FunctionBody() {
    auto m = Mark();

    CheckAndNext('(');

    ParamList();

    CheckAndNext(')');

    Block();

    CheckAndNext(TK_END);

    m.Complete(*this, LuaSyntaxNodeKind::FunctionBody);
}

void LuaParser::ParamList() {
    auto m = Mark();

    bool isVararg = false;
    if (Current() != ')') {
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
                case ')': {
                    break;
                }
                default: {
                    LuaExpectedError("<name> or '...' expected");
                    goto endLoop;
                }
            }
        } while (!isVararg && TestAndNext(','));
        endLoop:
        // empty stat
        void(0);
    }

    m.Complete(*this, LuaSyntaxNodeKind::ParamList);
}

/* suffixedexp ->
	 primaryexp { '.' NAME | '[' exp ']' | ':' NAME funcargs | funcargs }+ */
CompleteMarker LuaParser::SuffixedExpression() {
    auto m = Mark();
    auto cm = PrimaryExpression();
    bool suffix = false;
    for (;;) {
        switch (Current()) {
            case '.': {
                FieldSel();
                break;
            }
            case '[': {
                YIndex();
                break;
            }
            case ':': {
                FieldSel();
                FunctionCallArgs();
                break;
            }
            case '(':
            case TK_LONG_STRING:
            case TK_STRING:
            case '{': {
                FunctionCallArgs();
                break;
            }
            default:
                goto endLoop;
        }
        suffix = true;
    }
    endLoop:
    if (suffix) {
        return m.Complete(*this, LuaSyntaxNodeKind::SuffixedExpression);
    } else {
        m.Undo(*this);
        return cm;
    }
}

void LuaParser::FunctionCallArgs() {
    auto m = Mark();

    switch (Current()) {
        case '(': {
            Next();
            if (Current() != ')') {
                // extend grammar, allow pcall(1,2,)
                ExpressionList(')');
            }

            CheckAndNext(')');
            break;
        }
        case '{': {
            TableConstructor();
            break;
        }
        case TK_LONG_STRING:
        case TK_STRING: {
            auto sm = Mark();
            Next();
            sm.Complete(*this, LuaSyntaxNodeKind::StringLiteralExpression);
            break;
        }
        default: {
            LuaExpectedError("function arguments expected");
        }
    }

    m.Complete(*this, LuaSyntaxNodeKind::CallExpression);
}

/* fieldsel -> ['.' | ':'] NAME */
void LuaParser::FieldSel() {
    auto m = Mark();

    Next();
    CheckAndNext(TK_NAME);

    m.Complete(*this, LuaSyntaxNodeKind::IndexExpression);
}

/* index -> '[' expr ']' */
void LuaParser::YIndex() {
    auto m = Mark();

    CheckAndNext('[');
    Expression();
    CheckAndNext(']');

    m.Complete(*this, LuaSyntaxNodeKind::IndexExpression);
}

void LuaParser::FunctionName() {
    auto m = Mark();

    CheckAndNext(TK_NAME);

    while (Current() == '.') {
        FieldSel();
    }

    if (Current() == ':') {
        FieldSel();
    }

    m.Complete(*this, LuaSyntaxNodeKind::FunctionNameExpression);
}

std::string_view LuaParser::CheckName() {
    if (Current() == TK_NAME) {
        auto range = _tokens[_tokenIndex].Range;
        Next();
        return _file->GetSource().substr(range.StartOffset, range.EndOffset - range.StartOffset + 1);
    }

    LuaExpectedError("expected <name>");
    return "";
}

/* ATTRIB -> ['<' Name '>'] */
void LuaParser::LocalAttribute() {
    auto m = Mark();
    if (TestAndNext('<')) {
        auto attributeName = CheckName();
        CheckAndNext('>');
        if (attributeName != "const" && attributeName != "close") {
            LuaExpectedError(util::format("unknown attribute {}", attributeName));
        }
        m.Complete(*this, LuaSyntaxNodeKind::Attribute);
        return;
    }
    m.Undo(*this);
}

void LuaParser::Check(LuaTokenKind c) {
    if (Current() != c) {
        LuaExpectedError(util::format("{} expected", c));
    }
}

/* primaryexp -> NAME | '(' expr ')' */
CompleteMarker LuaParser::PrimaryExpression() {
    auto m = Mark();
    switch (Current()) {
        case '(': {
            Next();
            Expression();
            CheckAndNext(')');
            return m.Complete(*this, LuaSyntaxNodeKind::ParExpression);
        }
        case TK_NAME: {
            Next();
            return m.Complete(*this, LuaSyntaxNodeKind::NameExpression);
        }
        default:
            LuaExpectedError("unexpected symbol");
    }
    m.Undo(*this);
    return m.Complete(*this, LuaSyntaxNodeKind::None);
}

UnOpr LuaParser::GetUnaryOperator(LuaTokenKind op) {
    switch (op) {
        case TK_NOT: {
            return UnOpr::OPR_NOT;
        }
        case '-': {
            return UnOpr::OPR_MINUS;
        }
        case '~': {
            return UnOpr::OPR_BNOT;
        }
        case '#': {
            return UnOpr::OPR_LEN;
        }
        default: {
            return UnOpr::OPR_NOUNOPR;
        }
    }
}

BinOpr LuaParser::GetBinaryOperator(LuaTokenKind op) {
    switch (op) {
        case '+':
            return BinOpr::OPR_ADD;
        case '-':
            return BinOpr::OPR_SUB;
        case '*':
            return BinOpr::OPR_MUL;
        case '%':
            return BinOpr::OPR_MOD;
        case '^':
            return BinOpr::OPR_POW;
        case '/':
            return BinOpr::OPR_DIV;
        case TK_IDIV:
            return BinOpr::OPR_IDIV;
        case '&':
            return BinOpr::OPR_BAND;
        case '|':
            return BinOpr::OPR_BOR;
        case '~':
            return BinOpr::OPR_BXOR;
        case TK_SHL:
            return BinOpr::OPR_SHL;
        case TK_SHR:
            return BinOpr::OPR_SHR;
        case TK_CONCAT:
            return BinOpr::OPR_CONCAT;
        case TK_NE:
            return BinOpr::OPR_NE;
        case TK_EQ:
            return BinOpr::OPR_EQ;
        case '<':
            return BinOpr::OPR_LT;
        case TK_LE:
            return BinOpr::OPR_LE;
        case '>':
            return BinOpr::OPR_GT;
        case TK_GE:
            return BinOpr::OPR_GE;
        case TK_AND:
            return BinOpr::OPR_AND;
        case TK_OR:
            return BinOpr::OPR_OR;
        default:
            return BinOpr::OPR_NOBINOPR;
    }
}

void LuaParser::CheckAndNext(LuaTokenKind kind) {
    if (Current() != kind) {
        LuaExpectedError(util::format("token type {} expected", kind), kind);
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

void LuaParser::LuaExpectedError(std::string_view message, LuaTokenKind expectedToken) {
    auto me = MarkEvent(MarkEventType::Error);
    me.U.Error.ErrorKind = LuaParserErrorKind::Expect;
    me.U.Error.TokenKind = expectedToken;
    _events.push_back(me);
    if (_tokenIndex < _tokens.size()) {
        _errors.emplace_back(message, _tokens[_tokenIndex].Range, expectedToken);
    } else if (!_tokens.empty()) {
        auto tokenIndex = _tokens.size() - 1;
        _errors.emplace_back(message, _tokens[tokenIndex].Range, expectedToken);
    } else {
        _errors.emplace_back(message, TextRange(0, 0), expectedToken);
    }
}

void LuaParser::NameDefList() {
    auto m = Mark();

    CheckAndNext(TK_NAME);
    while (TestAndNext(',')) {
        CheckAndNext(TK_NAME);
    }

    m.Complete(*this, LuaSyntaxNodeKind::NameDefList);
}

std::vector<LuaToken> &LuaParser::GetTokens() {
    return _tokens;
}












