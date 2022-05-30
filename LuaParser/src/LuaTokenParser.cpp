#include "LuaParser/LuaTokenParser.h"
#include <limits>
#include <cstdlib>
#include "LuaDefine.h"
#include "LuaParser/LuaIdentify.h"
#include "LuaParser/LuaTokenTypeDetail.h"
#include "Util/format.h"
#include "Util/Utf8.h"

std::map<std::string, LuaTokenType, std::less<>> LuaTokenParser::LuaReserved = {
	{"and", TK_AND},
	{"break", TK_BREAK},
	{"do", TK_DO},
	{"else", TK_ELSE},
	{"elseif", TK_ELSEIF},
	{"end", TK_END},
	{"false", TK_FALSE},
	{"for", TK_FOR},
	{"function", TK_FUNCTION},
	{"goto", TK_GOTO},
	{"if", TK_IF},
	{"in", TK_IN},
	{"local", TK_LOCAL},
	{"nil", TK_NIL},
	{"not", TK_NOT},
	{"or", TK_OR},
	{"repeat", TK_REPEAT},
	{"return", TK_RETURN},
	{"then", TK_THEN},
	{"true", TK_TRUE},
	{"until", TK_UNTIL},
	{"while", TK_WHILE},
	{"//", TK_IDIV},
	{"..", TK_CONCAT},
	{"...", TK_DOTS},
	{"==", TK_EQ},
	{">=", TK_GE},
	{"<=", TK_LE},
	{"~=", TK_NE},
	{"<<", TK_SHL},
	{">>", TK_SHR},
	{"::", TK_DBCOLON}
};

LuaTokenParser::LuaTokenParser(std::shared_ptr<LuaFile> file)
	:
	_linenumber(0),
	_hasSaveText(false),
	_buffStart(0),
	_buffIndex(0),
	_hasEoz(false),
	_currentParseIndex(0),
	_currentIndex(0),
	_source(file->GetSource()),
	_eosToken(LuaToken{TK_EOS, "", TextRange(0, 0)}),
	_file(file),
	_customParser(nullptr)
{
}

bool LuaTokenParser::Parse()
{
	_file->Reset();
	while (true)
	{
		auto type = Lex();
		auto text = GetSaveText();

		if (!text.empty())
		{
			if (type == TK_LONG_COMMENT || type == TK_SHEBANG || type == TK_SHORT_COMMENT)
			{
				_commentTokens.emplace_back(type, text, TextRange(
					                            static_cast<int>(_buffStart), static_cast<int>(_buffIndex)));
			}
			else
			{
				_tokens.emplace_back(type, text, TextRange(
					                     static_cast<int>(_buffStart), static_cast<int>(_buffIndex)));
			}
		}
		else
		{
			break;
		}

		if (!_errors.empty())
		{
			_file->SetTotalLine(_linenumber);
			_file->UpdateLineInfo(_linenumber);
			return false;
		}
	}

	_file->SetTotalLine(_linenumber);
	return true;
}

LuaToken& LuaTokenParser::Next()
{
	if (_currentIndex < _tokens.size())
	{
		return _tokens[_currentIndex++];
	}

	return _eosToken;
}

LuaToken& LuaTokenParser::LookAhead()
{
	std::size_t nextIndex = _currentIndex + 1;

	if (nextIndex < _tokens.size())
	{
		return _tokens[nextIndex];
	}

	return _eosToken;
}

LuaToken& LuaTokenParser::Current()
{
	if (_currentIndex < _tokens.size())
	{
		return _tokens[_currentIndex];
	}

	return _eosToken;
}

int LuaTokenParser::LastValidOffset()
{
	if (!_tokens.empty())
	{
		return _tokens.back().Range.EndOffset;
	}
	return 0;
}

std::string_view LuaTokenParser::GetSource()
{
	return _source;
}

std::vector<LuaToken>& LuaTokenParser::GetComments()
{
	return _commentTokens;
}

std::vector<LuaError>& LuaTokenParser::GetErrors()
{
	return _errors;
}

bool LuaTokenParser::HasError() const
{
	return !_errors.empty();
}

void LuaTokenParser::ReleaseTokens()
{
	_tokens.clear();
	_tokens.shrink_to_fit();

	_commentTokens.clear();
	_commentTokens.shrink_to_fit();
}

std::shared_ptr<LuaFile> LuaTokenParser::GetFile()
{
	return _file;
}

bool LuaTokenParser::ConsumeAllTokens()
{
	return Current().TokenType == _eosToken.TokenType;
}

std::vector<LuaToken>& LuaTokenParser::GetTokens()
{
	return _tokens;
}

void LuaTokenParser::SetCustomParser(std::shared_ptr<LuaCustomParser> parser)
{
	_customParser = parser;
}

LuaTokenType LuaTokenParser::Lex()
{
	ResetBuffer();

	if (_customParser)
	{
		auto customType = CustomLex();
		if (customType != TK_UNKNOWN)
		{
			return customType;
		}
	}

	for (;;)
	{
		int ch = GetCurrentChar();
		switch (ch)
		{
		case '\n':
		case '\r':
			{
				IncLinenumber();
				break;
			}
		case ' ':
		case '\f':
		case '\t':
		case '\v':
			{
				NextChar();
				break;
			}
		case '-':
			{
				SaveAndNext();
				if (GetCurrentChar() != '-')
				{
					return '-';
				}
				// is comment
				SaveAndNext();

				if (GetCurrentChar() == '[')
				{
					std::size_t sep = SkipSep();
					if (sep >= 2)
					{
						ReadLongString(sep);
						return TK_LONG_COMMENT;
					}
				}

				// is short comment
				while (!CurrentIsNewLine() && GetCurrentChar() != EOZ)
				{
					SaveAndNext();
				}

				return TK_SHORT_COMMENT;
			}
		case '[':
			{
				std::size_t sep = SkipSep();
				if (sep >= 2)
				{
					ReadLongString(sep);
					return TK_STRING;
				}
				else if (sep == 0)
				{
					PushLuaError("invalid long string delimiter", TextRange(_currentParseIndex, _currentParseIndex));
					return TK_STRING;
				}
				return '[';
			}
		case '=':
			{
				SaveAndNext();
				if (CheckNext1('='))
				{
					return TK_EQ;
				}
				else
				{
					return '=';
				}
			}
		case '<':
			{
				SaveAndNext();
				if (CheckNext1('='))
				{
					return TK_LE;
				}
				else if (CheckNext1('<'))
				{
					return TK_SHL;
				}
				else
				{
					return '<';
				}
			}
		case '>':
			{
				SaveAndNext();
				if (CheckNext1('='))
				{
					return TK_GE;
				}
				else if (CheckNext1('>'))
				{
					return TK_SHR;
				}
				else
				{
					return '>';
				}
			}
		case '/':
			{
				SaveAndNext();
				if (CheckNext1('/'))
				{
					return TK_IDIV;
				}
				else
				{
					return '/';
				}
			}
		case '~':
			{
				SaveAndNext();
				if (CheckNext1('='))
				{
					return TK_NE;
				}
				else
				{
					return '~';
				}
			}
		case ':':
			{
				SaveAndNext();
				if (CheckNext1(':'))
				{
					return TK_DBCOLON;
				}
				else
				{
					return ':';
				}
			}
		case '"':
		case '\'':
			{
				ReadString(ch);
				return TK_STRING;
			}
		case '.':
			{
				SaveAndNext();
				if (CheckNext1('.'))
				{
					if (CheckNext1('.'))
					{
						return TK_DOTS; /* '...' */
					}
					else
					{
						return TK_CONCAT;
					}
				}
				else if (!lisdigit(GetCurrentChar()))
				{
					return '.';
				}
				else
				{
					return ReadNumeral();
				}
			}
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			{
				return ReadNumeral();
			}
		case EOZ:
			{
				return TK_EOS;
			}
		case '#':
			{
				SaveAndNext();
				// 只认为第一行的才是shebang
				if (_linenumber == 0 && _tokens.empty())
				{
					// shebang
					while (!CurrentIsNewLine() && GetCurrentChar() != EOZ)
					{
						SaveAndNext();
					}

					return TK_SHEBANG;
				}
				return '#';
			}
		default:
			{
				if (lislalpha(GetCurrentChar())) /* identifier or reserved word? */
				{
					do
					{
						SaveAndNext();
					}
					while (lislalnum(GetCurrentChar()));

					auto text = GetSaveText();

					if (IsReserved(text))
					{
						return LuaReserved.find(text)->second;
					}
					else
					{
						return TK_NAME;
					}
				}
				else /* single-char tokens ('+', '*', '%', '{', '}', ...) */
				{
					int c = GetCurrentChar();
					SaveAndNext();
					return c;
				}
			}
		}
	}
}

LuaTokenType LuaTokenParser::CustomLex()
{
	if (_customParser)
	{
		// skip space
		for (;;)
		{
			int ch = GetCurrentChar();
			switch (ch)
			{
			case '\n':
			case '\r':
				{
					IncLinenumber();
					break;
				}
			case ' ':
			case '\f':
			case '\t':
			case '\v':
				{
					NextChar();
					break;
				}
			case EOZ:
				{
					return TK_EOS;
				}
			default:
				{
					goto endSkip;
				}
			}
		}
	endSkip:

		std::size_t consumeSize = 0;
		auto type = _customParser->Lex(_source, _currentParseIndex, consumeSize);
		if (type != TK_UNKNOWN)
		{
			for (std::size_t i = 0; i != consumeSize; i++)
			{
				SaveAndNext();
			}
			return type;
		}
	}
	return TK_UNKNOWN;
}

int LuaTokenParser::NextChar()
{
	if (_currentParseIndex < (_source.size() - 1))
	{
		return _source[++_currentParseIndex];
	}
	else
	{
		++_currentParseIndex;
		return EOZ;
	}
}

void LuaTokenParser::SaveAndNext()
{
	Save();
	int ch = NextChar();
	if (ch == EOZ)
	{
		_hasEoz = true;
	}
}

void LuaTokenParser::Save()
{
	if (!_hasSaveText)
	{
		_hasSaveText = true;
		_buffStart = _currentParseIndex;
	}
	_buffIndex = _currentParseIndex;
}

int LuaTokenParser::GetCurrentChar()
{
	if (!_hasEoz && _currentParseIndex < _source.size())
	{
		unsigned char ch = _source[_currentParseIndex];
		return ch;
	}
	return EOZ;
}

bool LuaTokenParser::CheckNext1(int ch)
{
	if (_currentParseIndex < _source.size() && _source[_currentParseIndex] == ch)
	{
		SaveAndNext();
		return true;
	}
	return false;
}

/*
** Check whether current char is in set 'set' (with two chars) and
** saves it
*/
bool LuaTokenParser::CheckNext2(std::string_view set)
{
	if (set.size() > 2)
	{
		return false;
	}
	int ch = GetCurrentChar();
	if (ch == set[0] || ch == set[1])
	{
		SaveAndNext();
		return true;
	}
	return false;
}

/* LUA_NUMBER */
/*
** This function is quite liberal in what it accepts, as 'luaO_str2num'
** will reject ill-formed numerals. Roughly, it accepts the following
** pattern:
**
**   %d(%x|%.|([Ee][+-]?))* | 0[Xx](%x|%.|([Pp][+-]?))*
**
** The only tricky part is to accept [+-] only after a valid exponent
** mark, to avoid reading '3-4' or '0xe+1' as a single number.
**
** The caller might have already read an initial dot.
*/
LuaTokenType LuaTokenParser::ReadNumeral()
{
	int first = GetCurrentChar();
	const char* expo = "Ee";
	SaveAndNext();

	if (first == '0' && CheckNext2("xX")) /* hexadecimal? */
	{
		expo = "Pp";
	}

	for (;;)
	{
		if (CheckNext2(expo)) /* exponent mark? */
		{
			CheckNext2("-+"); /* optional exponent sign */
		}
		else if (lisxdigit(GetCurrentChar()) || GetCurrentChar() == '.') /* '%x|%.' */
		{
			SaveAndNext();
		}
		else
		{
			break;
		}
	}

	// fix bug:这里不能使用lislalpha,否则会错误的解析下一个unicode字符
	if (std::isalpha(GetCurrentChar())) /* is numeral touching a letter? */
	{
		// luajit
		if (CheckNext1('U'))
		{
			if (CheckNext1('L') && CheckNext1('L'))
			{
				return TK_INT;
			}
			else
			{
				PushLuaError("unknown integer type", TextRange(_currentParseIndex, _currentParseIndex));
			}
		}
		else
		{
			SaveAndNext();
		}
	}

	// todo error format check
	// 实际上应该是TK_NUMBER
	return TK_INT;
}

/*
** read a sequence '[=*[' or ']=*]', leaving the last bracket. If
** sequence is well formed, return its number of '='s + 2; otherwise,
** return 1 if it is a single bracket (no '='s and no 2nd bracket);
** otherwise (an unfinished '[==...') return 0.
*/
std::size_t LuaTokenParser::SkipSep()
{
	std::size_t count = 0;
	int ch = GetCurrentChar();

	SaveAndNext();

	while (GetCurrentChar() == '=')
	{
		SaveAndNext();
		count++;
	}

	return GetCurrentChar() == ch
		       ? count + 2
		       : (count == 0)
		       ? 1
		       : 0;
}

void LuaTokenParser::ReadLongString(std::size_t sep)
{
	SaveAndNext();

	if (CurrentIsNewLine())
	{
		IncLinenumber();
	}

	for (;;)
	{
		switch (GetCurrentChar())
		{
		case EOZ:
			{
				PushLuaError("unfinished long string starting", TextRange(_currentParseIndex, _currentParseIndex));
				return;
			}
		case ']':
			{
				if (SkipSep() == sep)
				{
					SaveAndNext();
					return;
				}
				break;
			}
		case '\n':
		case '\r':
			{
				Save();
				IncLinenumber();
				break;
			}
		default:
			{
				SaveAndNext();
			}
		}
	}
}

void LuaTokenParser::ReadString(int del)
{
	SaveAndNext();
	while (GetCurrentChar() != del)
	{
		switch (GetCurrentChar())
		{
		case EOZ:
		case '\n':
		case '\r':
			{
				PushLuaError("unfinished string", TextRange(_currentParseIndex, _currentParseIndex));
				return;
			}
		case '\\':
			{
				SaveAndNext();

				switch (GetCurrentChar())
				{
				case EOZ:
					PushLuaError("unfinished string", TextRange(_currentParseIndex, _currentParseIndex));
					return;
				case 'z':
					{
						SaveAndNext();
						while (lisspace(GetCurrentChar()))
						{
							if (CurrentIsNewLine())
							{
								IncLinenumber();
							}
							else
							{
								SaveAndNext();
							}
						}
						goto no_save;
					}
				case '\r':
				case '\n':
					{
						if (CurrentIsNewLine())
						{
							IncLinenumber();
						}
						goto no_save;
					}
				}
				break;
			}
		}
		SaveAndNext();
		// 空语句
	no_save:;
	}
	SaveAndNext();
}


void LuaTokenParser::IncLinenumber()
{
	int old = GetCurrentChar();

	NextChar();

	if (CurrentIsNewLine() && GetCurrentChar() != old)
	{
		NextChar(); /* skip '\n\r' or '\r\n' */
		_file->SetEndOfLineState(EndOfLine::CRLF);
	}
	else if (old == '\n')
	{
		_file->SetEndOfLineState(EndOfLine::LF);
	}
	else
	{
		_file->SetEndOfLineState(EndOfLine::CR);
	}


	if (++_linenumber >= std::numeric_limits<int>::max())
	{
		return;
	}

	_file->PushLine(static_cast<int>(_currentParseIndex));
}

bool LuaTokenParser::CurrentIsNewLine()
{
	int ch = GetCurrentChar();
	return ch == '\n' || ch == '\r';
}

void LuaTokenParser::ResetBuffer()
{
	_hasSaveText = false;
	_buffStart = 0;
	_buffIndex = 0;
}

std::string_view LuaTokenParser::GetSaveText() const
{
	if (_hasSaveText)
	{
		return std::string_view(_source.data() + _buffStart, _buffIndex - _buffStart + 1);
	}
	else
	{
		return std::string_view(_source.data() + _buffStart, 0);
	}
}

bool LuaTokenParser::IsReserved(std::string_view text)
{
	return LuaReserved.find(text) != LuaReserved.end();
}

void LuaTokenParser::PushLuaError(std::string_view message, TextRange range)
{
	_errors.emplace_back(message, range);
}
