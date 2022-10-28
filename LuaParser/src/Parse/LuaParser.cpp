﻿#include "LuaParser/Parse/LuaParser.h"
#include <fstream>
#include "Lexer/LuaDefine.h"
#include "LuaParser/Lexer/LuaTokenTypeDetail.h"
#include "LuaParser/Parse/LuaOperatorType.h"
#include "Util/format.h"
#include "LuaParser/exception/LuaParseException.h"
#include "LuaParser/File/LuaFile.h"

std::shared_ptr<LuaParser> LuaParser::LoadFromFile(std::string_view filename) {
    std::string realPath(filename);
    std::fstream fin(realPath, std::ios::in | std::ios::binary);

    if (fin.is_open()) {
        std::stringstream s;
        s << fin.rdbuf();
        auto parser = LoadFromBuffer(s.str());
        return parser;
    }

    return nullptr;
}

std::shared_ptr<LuaParser> LuaParser::LoadFromBuffer(std::string &&buffer) {
    auto file = std::make_shared<LuaFile>(std::move(buffer));
    LuaLexer luaLexer(file);
    luaLexer.Parse();
    return std::make_shared<LuaParser>(file, std::move(luaLexer.GetTokens()));
}

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

//
//void LuaParser::BuildAstWithComment() {
//    BuildAst();
//
//    auto &chunkChildren = _chunkAstNode->GetChildren();
//    auto &comments = _tokenParser->GetComments();
//    if (chunkChildren.empty()) {
//        if (comments.empty()) {
//            return;
//        }
//        auto block = CreateAstNode(LuaNodeType::Block);
//        chunkChildren.push_back(block);
//    }
//
//    auto blockNode = chunkChildren.front();
//
//
//    // 将注释注入AST中
//    if (!comments.empty()) {
//        for (auto &comment: comments) {
//            std::shared_ptr<LuaAstNode> commentAst = CreateAstNode(LuaNodeType::Comment);
//            switch (comment.TokenType) {
//                case TK_SHORT_COMMENT: {
//                    commentAst->AddChild(CreateAstNodeFromToken(LuaNodeType::ShortComment, comment));
//                    break;
//                }
//                case TK_LONG_COMMENT: {
//                    commentAst->AddChild(CreateAstNodeFromToken(LuaNodeType::LongComment, comment));
//                    break;
//                }
//                case TK_SHEBANG: {
//                    commentAst->AddChild(CreateAstNodeFromToken(LuaNodeType::ShebangComment, comment));
//                    break;
//                }
//                case TK_DOC_COMMENT: {
//                    auto shortComment = CreateAstNodeFromToken(LuaNodeType::ShortComment, comment);
//                    DocStatement(shortComment, comment);
//                    commentAst->AddChild(shortComment);
//                    break;
//                }
//                default: {
//                    break;
//                }
//            }
//
//            blockNode->AddComment(commentAst);
//        }
//    }
//}
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

LuaTokenType LuaParser::LookAhead() {
    std::size_t nextIndex = _tokenIndex + 1;

    while (nextIndex < _tokens.size()) {
        auto tk = _tokens[nextIndex].TokenType;
        switch (tk) {
            case TK_SHORT_COMMENT:
            case TK_LONG_COMMENT:
            case TK_SHEBANG: {
                nextIndex++;
            }
            default: {
                return tk;
            }
        }
    }

    return TK_EOF;
}

LuaTokenType LuaParser::Current() {
    if (_invalid) {
        _invalid = false;
        SkipComment();
        if (_tokenIndex < _tokens.size()) {
            _current = _tokens[_tokenIndex].TokenType;
        } else {
            _current = TK_EOF;
        }
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
            m.Complete(*this, LuaNodeType::EmptyStatement);
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

    m.Complete(*this, LuaNodeType::IfStatement);
}

/* whilestat -> WHILE cond DO block END */
void LuaParser::WhileStatement() {
    auto m = Mark();

    CheckAndNext(TK_WHILE);

    Condition();

    CheckAndNext(TK_DO);

    Block();

    CheckAndNext(TK_END);

    m.Complete(*this, LuaNodeType::WhileStatement);
}

void LuaParser::DoStatement() {
    auto m = Mark();

    CheckAndNext(TK_DO);

    Block();

    CheckAndNext(TK_END);

    m.Complete(*this, LuaNodeType::DoStatement);
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
            ThrowLuaError("'=' or 'in' expected");
        }
    }

    m.Complete(*this, LuaNodeType::ForStatement);
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

    m.Complete(*this, LuaNodeType::ForNumber);
}

/* forlist -> NAME {,NAME} IN explist forbody */
void LuaParser::ForList() {
    auto m = Mark();

    NameDefList();

    CheckAndNext(TK_IN);

    ExpressionList();

    ForBody();

    m.Complete(*this, LuaNodeType::ForList);
}

void LuaParser::ForBody() {
    auto m = Mark();

    CheckAndNext(TK_DO);

    Block();

    CheckAndNext(TK_END);

    m.Complete(*this, LuaNodeType::ForBody);
}

/* repeatstat -> REPEAT block UNTIL cond */
void LuaParser::RepeatStatement() {
    auto m = Mark();

    CheckAndNext(TK_REPEAT);

    Block();

    CheckAndNext(TK_UNTIL);

    Condition();

    m.Complete(*this, LuaNodeType::RepeatStatement);
}

void LuaParser::FunctionStatement() {
    auto m = Mark();

    CheckAndNext(TK_FUNCTION);

    FunctionName();

    FunctionBody();

    m.Complete(*this, LuaNodeType::FunctionStatement);
}

void LuaParser::LocalFunctionStatement() {
    auto m = Mark();

    CheckAndNext(TK_LOCAL);

    CheckAndNext(TK_FUNCTION);

    CheckName();

    FunctionBody();

    m.Complete(*this, LuaNodeType::LocalFunctionStatement);
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
    nm.Complete(*this, LuaNodeType::NameDefList);

    if (TestAndNext('=')) {
        ExpressionList();
    }
    // 如果有一个分号则加入到localstatement
    TestAndNext(';');

    m.Complete(*this, LuaNodeType::LocalStatement);
}

void LuaParser::LabelStatement() {
    auto m = Mark();

    CheckAndNext(TK_DBCOLON);

    CheckName();

    CheckAndNext(TK_DBCOLON);

    m.Complete(*this, LuaNodeType::LabelStatement);
}

void LuaParser::ReturnStatement() {
    auto m = Mark();

    CheckAndNext(TK_RETURN);

    if (!(BlockFollow() || Current() == ';')) {
        ExpressionList();
    }

    TestAndNext(';');

    m.Complete(*this, LuaNodeType::ReturnStatement);
}


void LuaParser::BreakStatement() {
    auto m = Mark();

    CheckAndNext(TK_BREAK);

    TestAndNext(';');

    m.Complete(*this, LuaNodeType::BreakStatement);
}

void LuaParser::GotoStatement() {
    auto m = Mark();

    CheckAndNext(TK_GOTO);

    CheckName();

    TestAndNext(';');

    m.Complete(*this, LuaNodeType::GotoStatement);
}

/* stat -> func | assignment */
/*
 * 以上是lua定义
 * 以下是修改定义
 * exprstat -> func
 * assignment -> exprList '=' exprList
 */
void LuaParser::ExpressionStatement() {
    auto m = Mark();
    SuffixedExpression();
    if (Current() == '=' || Current() == ',') {
        while (TestAndNext(',')) {
            SuffixedExpression();
        }
        auto cm = m.Complete(*this, LuaNodeType::VarList);
        m = cm.Precede(*this);

        CheckAndNext('=');

        ExpressionList();

        // 如果发现一个分号，会认为分号为该语句的结尾
        TestAndNext(';');
        m.Complete(*this, LuaNodeType::AssignStatement);
    } else {
        TestAndNext(';');
        m.Complete(*this, LuaNodeType::ExpressionStatement);
    }
}

//
//void LuaParser::DocStatement(std::shared_ptr<LuaAstNode> comment, LuaToken &docComment) {
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
//void LuaParser::DocTagFormatStatement(std::shared_ptr<LuaAstNode> comment, LuaDocTokenParser &docTokenParser) {
//    auto docFormat = CreateAstNodeFromToken(LuaNodeType::DocTagFormat, docTokenParser.Current());
//
//    docTokenParser.Next();
//    switch (docTokenParser.Current().TokenType) {
//        case TK_DOC_DISABLE:
//        case TK_DOC_DISABLE_NEXT: {
//            docFormat->AddChild(CreateAstNodeFromToken(LuaNodeType::KeyWord, docTokenParser.Current()));
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

    m.Complete(*this, LuaNodeType::Block);
}

/* explist -> expr { ',' expr } */
void LuaParser::ExpressionList(LuaTokenType stopToken) {
    auto m = Mark();

    Expression();
    while (TestAndNext(',')) {
        if (Current() == stopToken) {
            break;
        }

        Expression();
    }

    m.Complete(*this, LuaNodeType::ExpressionList);
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
        cm = m.Complete(*this, LuaNodeType::UnaryExpression);
    } else {
        cm = SimpleExpression();
    }

    auto op = GetBinaryOperator(Current());
    /* expand while operators have priorities higher than 'limit' */
    while (op != BinOpr::OPR_NOBINOPR && priority[static_cast<int>(op)].left > limit) {
        auto m = cm.Precede(*this);

        Next();

        Subexpression(priority[static_cast<int>(op)].right);

        cm = m.Complete(*this, LuaNodeType::BinaryExpression);
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
            return m.Complete(*this, LuaNodeType::LiteralExpression);
        }
        case TK_STRING: {
            auto m = Mark();
            Next();
            return m.Complete(*this, LuaNodeType::StringLiteralExpression);
        }
        case '{': {
            return TableConstructor();
        }
        case TK_FUNCTION: {
            auto m = Mark();

            Next();
            FunctionBody();

            return m.Complete(*this, LuaNodeType::ClosureExpression);
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

    do {
        if (Current() == '}') {
            break;
        }
        Field();
    } while (TestAndNext(',')
             || TestAndNext(';'));

    CheckAndNext('}');

    return m.Complete(*this, LuaNodeType::TableExpression);
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

    m.Complete(*this, LuaNodeType::TableField);
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

    ParamList();

    Block();

    CheckAndNext(TK_END);

    m.Complete(*this, LuaNodeType::FunctionBody);
}

void LuaParser::ParamList() {
    auto m = Mark();

    CheckAndNext('(');

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
                    ThrowLuaError("<name> or '...' expected");
                }
            }
        } while (!isVararg && TestAndNext(','));
    }

    CheckAndNext(')');

    m.Complete(*this, LuaNodeType::ParamList);
}

/* suffixedexp ->
	 primaryexp { '.' NAME | '[' exp ']' | ':' NAME funcargs | funcargs } */
CompleteMarker LuaParser::SuffixedExpression() {
    auto m = Mark();
    PrimaryExpression();
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
            case TK_STRING:
            case '{': {
                FunctionCallArgs();
                break;
            }
            default:
                goto endLoop;
        }
    }
    endLoop:

    return m.Complete(*this, LuaNodeType::SuffixedExpression);
}

void LuaParser::FunctionCallArgs() {
    auto m = Mark();

    switch (Current()) {
        case '(': {
            Next();
            if (Current() != ')') {
                // extend grammar, allow pcall(1,2,)
                ExpressionList();
            }

            CheckAndNext(')');
            break;
        }
        case '{': {
            TableConstructor();
            break;
        }
        case TK_STRING: {
            auto sm = Mark();
            Next();
            sm.Complete(*this, LuaNodeType::StringLiteralExpression);
            break;
        }
        default: {
            ThrowLuaError("function arguments expected");
        }
    }

    m.Complete(*this, LuaNodeType::CallExpression);
}

/* fieldsel -> ['.' | ':'] NAME */
void LuaParser::FieldSel() {
    auto m = Mark();

    Next();
    CheckAndNext(TK_NAME);

    m.Complete(*this, LuaNodeType::IndexExpression);
}

/* index -> '[' expr ']' */
void LuaParser::YIndex() {
    auto m = Mark();

    CheckAndNext('[');
    Expression();
    CheckAndNext(']');

    m.Complete(*this, LuaNodeType::IndexExpression);
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

    m.Complete(*this, LuaNodeType::FunctionNameExpression);
}

std::string_view LuaParser::CheckName() {
    Check(TK_NAME);

//    auto identify = CreateAstNodeFromCurrentToken(LuaNodeType::Identify);
//
//    parent->AddChild(identify);
//
//    _tokenParser->Next();

//    return identify->GetText();
}

/* ATTRIB -> ['<' Name '>'] */
void LuaParser::LocalAttribute() {
    if (TestAndNext('<')) {
        auto m = Mark();
        auto attributeName = CheckName();
        CheckAndNext('>');
        if (attributeName != "const" && attributeName != "close") {
            ThrowMatchError(Util::format("unknown attribute {}", attributeName));
        }
        m.Complete(*this, LuaNodeType::Attribute);
    }
}

void LuaParser::Check(LuaTokenType c) {
    if (Current() != c) {
        ThrowMatchError(Util::format("{} expected"));
    }
}

/* primaryexp -> NAME | '(' expr ')' */
void LuaParser::PrimaryExpression() {
    auto m = Mark();
    switch (Current()) {
        case '(': {
            Next();
            Expression();
            CheckAndNext(')');
            m.Complete(*this, LuaNodeType::ParExpression);
            break;
        }
        case TK_NAME: {
            Next();
            m.Complete(*this, LuaNodeType::NameExpression);
            break;
        }
        default:
            ThrowLuaError("unexpected symbol");
    }
}

UnOpr LuaParser::GetUnaryOperator(LuaTokenType op) {
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

BinOpr LuaParser::GetBinaryOperator(LuaTokenType op) {
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

void LuaParser::CheckAndNext(LuaTokenType c) {
    if (Current() != c) {
        ThrowLuaError(Util::format("token type {} expected", c));
        return;
    }

    Next();
}

bool LuaParser::TestAndNext(LuaTokenType c) {
    if (Current() == c) {
        Next();
        return true;
    }
    return false;
}

void LuaParser::ThrowLuaError(std::string_view message) {
//    if (Current() != TK_EOF) {
//        Next();
//        _errors.emplace_back(message, 0);
//    } else {
//        auto offset = _tokenParser->LastValidOffset();
//        if (offset != 0) {
//            _errors.emplace_back(message, TextRange(offset, offset), 0);
//        }
//    }
//    throw LuaParseException(message);
}

void LuaParser::ThrowMatchError(std::string message) {
//    _errors.emplace_back(message, range, token);
//    throw LuaParseException(message);
}

void LuaParser::NameDefList() {
    auto m = Mark();

    CheckAndNext(TK_NAME);
    while (TestAndNext(',')) {
        CheckAndNext(TK_NAME);
    }

    m.Complete(*this, LuaNodeType::NameDefList);
}

std::vector<LuaToken> &LuaParser::GetTokens() {
    return _tokens;
}









