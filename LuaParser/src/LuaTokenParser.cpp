#include "LuaParser/LuaTokenParser.h"
#include <limits>
#include "LuaDefine.h"
#include "LuaTokenTypeDetail.h"
#include "Util/format.h"

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

LuaTokenParser::LuaTokenParser(std::string&& source)
	: _currentParseIndex(0),
	  _currentIndex(0),
	  _source(source),
	  _hasSaveText(false),
	  _buffStart(0),
	  _buffIndex(0),
	  _linenumber(0),
	  _hasEoz(false),
	  _eosToken(LuaToken{TK_EOS, "", TextRange(0, 0)})
{
	_lineOffsetVec.push_back(0);
}

bool LuaTokenParser::Parse()
{

	while (true)
	{
		auto type = llex();
		auto text = getSaveText();

		if (!text.empty())
		{
			if (type == TK_LONG_COMMENT || type == TK_SHEBANG || type == TK_SHORT_COMMENT)
			{
				_commentTokens.emplace_back(type, text, TextRange(_buffStart, _buffIndex));
			}
			else
			{
				_tokens.emplace_back(type, text, TextRange(_buffStart, _buffIndex));
			}
		}
		else
		{
			break;
		}
	}
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
	if (_currentIndex + 1 < _tokens.size())
	{
		return _tokens[_currentIndex + 1];
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
		return _tokens.back().LuaTextRange.EndOffset;
	}
	return 0;
}


int LuaTokenParser::GetLine(int offset)
{
	if (_lineOffsetVec.empty())
	{
		return 0;
	}

	int maxLine = static_cast<int>(_lineOffsetVec.size()) - 1;
	int targetLine = maxLine;
	int upperLine = maxLine;
	int lowestLine = 0;

	while (true)
	{
		if (_lineOffsetVec[targetLine] > offset)
		{
			upperLine = targetLine;

			targetLine = (upperLine + lowestLine) / 2;

			if (targetLine == 0)
			{
				return targetLine;
			}
		}
		else
		{
			if (upperLine - targetLine <= 1)
			{
				return targetLine;
			}

			lowestLine = targetLine;

			targetLine = (upperLine + lowestLine) / 2;
		}
	}

	return 0;
}

int LuaTokenParser::GetColumn(int offset)
{
	int line = GetLine(offset);

	int lineStartOffset = _lineOffsetVec[line];

	return offset - lineStartOffset;
}

std::string& LuaTokenParser::GetSource()
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

LuaTokenType LuaTokenParser::llex()
{
	resetBuffer();

	for (;;)
	{
		int ch = getCurrentChar();
		switch (ch)
		{
		case '\n':
		case '\r':
			{
				incLinenumber();
				break;
			}
		case ' ':
		case '\f':
		case '\t':
		case '\v':
			{
				nextChar();
				break;
			}
		case '-':
			{
				saveAndNext();
				if (getCurrentChar() != '-')
				{
					return '-';
				}
				// is comment
				saveAndNext();

				if (getCurrentChar() == '[')
				{
					std::size_t sep = skipSep();
					if (sep >= 2)
					{
						readLongString(sep);
						return TK_LONG_COMMENT;
					}
				}

				// is short comment
				while (!currentIsNewLine() && getCurrentChar() != EOZ)
				{
					saveAndNext();
				}

				return TK_SHORT_COMMENT;
			}
		case '[':
			{
				std::size_t sep = skipSep();
				if (sep >= 2)
				{
					readLongString(sep);
					return TK_STRING;
				}
				else if (sep == 0)
				{
					luaError("invalid long string delimiter", TextRange(_currentParseIndex, _currentParseIndex));
					return TK_STRING;
				}
				return '[';
			}
		case '=':
			{
				saveAndNext();
				if (checkNext1('='))
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
				saveAndNext();
				if (checkNext1('='))
				{
					return TK_LE;
				}
				else if (checkNext1('<'))
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
				saveAndNext();
				if (checkNext1('='))
				{
					return TK_GE;
				}
				else if (checkNext1('>'))
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
				saveAndNext();
				if (checkNext1('/'))
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
				saveAndNext();
				if (checkNext1('='))
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
				saveAndNext();
				if (checkNext1(':'))
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
				readString(ch);
				return TK_STRING;
			}
		case '.':
			{
				saveAndNext();
				if (checkNext1('.'))
				{
					if (checkNext1('.'))
					{
						return TK_DOTS; /* '...' */
					}
					else
					{
						return TK_CONCAT;
					}
				}
				else if (!lisdigit(getCurrentChar()))
				{
					return '.';
				}
				else
				{
					return readNumeral();
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
				return readNumeral();
			}
		case EOZ:
			{
				return TK_EOS;
			}
		case '#':
			{
				saveAndNext();
				// 只认为第一行的才是shebang
				if (_linenumber == 0 && getCurrentChar() == '!')
				{
					// shebang
					while (!currentIsNewLine() && getCurrentChar() != EOZ)
					{
						saveAndNext();
					}

					return TK_SHEBANG;
				}
				return '#';
			}
		default:
			{
				if (lislalpha(getCurrentChar())) /* identifier or reserved word? */
				{
					do
					{
						saveAndNext();
					}
					while (lislalnum(getCurrentChar()));

					auto text = getSaveText();

					if (isReserved(text))
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
					int c = getCurrentChar();
					saveAndNext();
					return c;
				}
			}
		}
	}
}

int LuaTokenParser::nextChar()
{
	if (_currentParseIndex < _source.size())
	{
		return _source[++_currentParseIndex];
	}
	else
	{
		return EOZ;
	}
}

void LuaTokenParser::saveAndNext()
{
	save();
	int ch = nextChar();
	if (ch == EOZ)
	{
		_hasEoz = true;
	}
}

void LuaTokenParser::save()
{
	if (!_hasSaveText)
	{
		_hasSaveText = true;
		_buffStart = _currentParseIndex;
	}
	_buffIndex = _currentParseIndex;
}

int LuaTokenParser::getCurrentChar()
{
	if (!_hasEoz && _currentParseIndex < _source.size())
	{
		unsigned char ch = _source[_currentParseIndex];
		return ch;
	}
	return EOZ;
}

bool LuaTokenParser::checkNext1(int ch)
{
	if (_source[_currentParseIndex] == ch)
	{
		saveAndNext();
		return true;
	}
	return false;
}

/*
** Check whether current char is in set 'set' (with two chars) and
** saves it
*/
bool LuaTokenParser::checkNext2(std::string_view set)
{
	if (set.size() > 2)
	{
		return false;
	}
	int ch = getCurrentChar();
	if (ch == set[0] || ch == set[1])
	{
		saveAndNext();
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
LuaTokenType LuaTokenParser::readNumeral()
{
	int first = getCurrentChar();
	const char* expo = "Ee";
	saveAndNext();

	if (first == '0' && checkNext2("xX")) /* hexadecimal? */
	{
		expo = "Pp";
	}

	for (;;)
	{
		if (checkNext2(expo)) /* exponent mark? */
		{
			checkNext2("-+"); /* optional exponent sign */
		}
		else if (lisxdigit(getCurrentChar()) || getCurrentChar() == '.') /* '%x|%.' */
		{
			saveAndNext();
		}
		else
		{
			break;
		}
	}

	if (lislalpha(getCurrentChar())) /* is numeral touching a letter? */
	{
		saveAndNext();
	}

	// todo error format check

	return TK_INT;
}

/*
** read a sequence '[=*[' or ']=*]', leaving the last bracket. If
** sequence is well formed, return its number of '='s + 2; otherwise,
** return 1 if it is a single bracket (no '='s and no 2nd bracket);
** otherwise (an unfinished '[==...') return 0.
*/
std::size_t LuaTokenParser::skipSep()
{
	std::size_t count = 0;
	int ch = getCurrentChar();

	saveAndNext();

	while (getCurrentChar() == '=')
	{
		saveAndNext();
		count++;
	}

	return getCurrentChar() == ch
		       ? count + 2
		       : (count == 0)
		       ? 1
		       : 0;
}

void LuaTokenParser::readLongString(std::size_t sep)
{
	saveAndNext();

	if (currentIsNewLine())
	{
		incLinenumber();
	}

	for (;;)
	{
		switch (getCurrentChar())
		{
		case EOZ:
			{
				luaError("unfinished long string starting", TextRange(_currentParseIndex, _currentParseIndex));
				return;
			}
		case ']':
			{
				if (skipSep() == sep)
				{
					saveAndNext();
					return;
				}
				break;
			}
		case '\n':
		case '\r':
			{
				save();
				incLinenumber();
				break;
			}
		default:
			{
				saveAndNext();
			}
		}
	}
}

void LuaTokenParser::readString(int del)
{
	saveAndNext();
	while (getCurrentChar() != del)
	{
		switch (getCurrentChar())
		{
		case EOZ:
		case '\n':
		case '\r':
			{
				luaError("unfinished string", TextRange(_currentParseIndex, _currentParseIndex));
				return;
			}
		case '\\':
			{
				saveAndNext();

				switch (getCurrentChar())
				{
				case EOZ:
					luaError("unfinished string", TextRange(_currentParseIndex, _currentParseIndex));
					return;
				}
				break;
			}
		}
		saveAndNext();
	}
	saveAndNext();
}


void LuaTokenParser::incLinenumber()
{
	int old = getCurrentChar();

	nextChar();

	if (currentIsNewLine() && getCurrentChar() != old)
	{
		nextChar(); /* skip '\n\r' or '\r\n' */
	}

	if (++_linenumber >= std::numeric_limits<int>::max())
	{
		return;
		// luaError("")
		// throw LuaParserException("chunk has too many lines");
	}

	_lineOffsetVec.push_back(static_cast<int>(_currentParseIndex));
}

bool LuaTokenParser::currentIsNewLine()
{
	int ch = getCurrentChar();
	return ch == '\n' || ch == '\r';
}

void LuaTokenParser::resetBuffer()
{
	_hasSaveText = false;
	_buffStart = 0;
	_buffIndex = 0;
}

std::string_view LuaTokenParser::getSaveText() const
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

bool LuaTokenParser::isReserved(std::string_view text)
{
	return LuaReserved.find(text) != LuaReserved.end();
}

void LuaTokenParser::luaError(std::string_view message, TextRange range)
{
	_errors.emplace_back(message, range);
}
