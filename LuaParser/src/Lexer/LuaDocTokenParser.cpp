//#include "LuaParser/LuaDocTokenParser.h"
//
//#include "LuaDefine.h"
//#include "LuaParser/LuaTokenTypeDetail.h"
//
//std::map<std::string, LuaTokenType, std::less<>> LuaDocTokenParser::LuaDocReserved = {
//	{"@format", TK_DOC_TAG_FORMAT},
//	{"disable", TK_DOC_DISABLE},
//	{"disable-next", TK_DOC_DISABLE_NEXT}
//};
//
//LuaDocTokenParser::LuaDocTokenParser(std::string_view doc, TextRange range)
//	: _source(doc),
//	  _range(range),
//	  _current(LuaToken{TK_EOS, "", TextRange(0, 0)}),
//	  _hasSaveText(false),
//	  _buffStart(0),
//	  _buffIndex(0),
//	  _hasEoz(false),
//	  _currentParseIndex(0),
//	  _docTagFounded(false)
//{
//}
//
//LuaDocTokenParser::~LuaDocTokenParser()
//{
//}
//
//LuaToken& LuaDocTokenParser::Current()
//{
//	return _current;
//}
//
//void LuaDocTokenParser::Next()
//{
//	LuaTokenType type = 0;
//	if (_docTagFounded)
//	{
//		type = Lex();
//	}
//	else
//	{
//		type = TagLex();
//	}
//
//	auto text = GetSaveText();
//
//	_current = LuaToken(type, text,
//	                    TextRange(_range.StartOffset + _buffStart, _range.StartOffset + _buffIndex));
//}
//
//LuaTokenType LuaDocTokenParser::Lex()
//{
//	ResetBuffer();
//
//	// skip space
//	for (;;)
//	{
//		int ch = GetCurrentChar();
//		switch (ch)
//		{
//		case ' ':
//		case '\f':
//		case '\t':
//		case '\v':
//			{
//				NextChar();
//				break;
//			}
//		case EOZ:
//			{
//				return TK_EOS;
//			}
//		default:
//			{
//				if (lislalpha(GetCurrentChar())) /* identifier or reserved word? */
//				{
//					do
//					{
//						SaveAndNext();
//					}
//					while (lislalnum(GetCurrentChar()) || GetCurrentChar() == '-');
//
//					auto text = GetSaveText();
//
//					auto it = LuaDocReserved.find(text);
//					if (it != LuaDocReserved.end())
//					{
//						return it->second;
//					}
//					else
//					{
//						return TK_NAME;
//					}
//				}
//				else /* single-char tokens ('+', '*', '%', '{', '}', ...) */
//				{
//					int c = GetCurrentChar();
//					SaveAndNext();
//					return c;
//				}
//			}
//		}
//	}
//
//	return TK_EOS;
//}
//
//LuaTokenType LuaDocTokenParser::TagLex()
//{
//	ResetBuffer();
//
//	for (int i = 0; i != 3; i++)
//	{
//		if (GetCurrentChar() != '-')
//		{
//			return TK_EOS;
//		}
//		NextChar();
//	}
//
//	for (;;)
//	{
//		int ch = GetCurrentChar();
//		switch (ch)
//		{
//		case ' ':
//		case '\f':
//		case '\t':
//		case '\v':
//			{
//				NextChar();
//				break;
//			}
//		case EOZ:
//			{
//				return TK_EOS;
//			}
//		case '@':
//			{
//				SaveAndNext();
//				if (lislalpha(GetCurrentChar())) /* identifier or reserved word? */
//				{
//					do
//					{
//						SaveAndNext();
//					}
//					while (lislalnum(GetCurrentChar()));
//
//					auto text = GetSaveText();
//
//					auto it = LuaDocReserved.find(text);
//					if (it != LuaDocReserved.end())
//					{
//						_docTagFounded = true;
//						return it->second;
//					}
//				}
//				return TK_EOS;
//			}
//		default:
//			{
//				return TK_EOS;
//			}
//		}
//	}
//
//	return TK_EOS;
//}
//
//int LuaDocTokenParser::NextChar()
//{
//	if (_currentParseIndex < (_source.size() - 1))
//	{
//		return _source[++_currentParseIndex];
//	}
//	else
//	{
//		++_currentParseIndex;
//		return EOZ;
//	}
//}
//
//void LuaDocTokenParser::SaveAndNext()
//{
//	if (!_hasSaveText)
//	{
//		_hasSaveText = true;
//		_buffStart = _currentParseIndex;
//	}
//	_buffIndex = _currentParseIndex;
//
//	int ch = NextChar();
//	if (ch == EOZ)
//	{
//		_hasEoz = true;
//	}
//}
//
//int LuaDocTokenParser::GetCurrentChar()
//{
//	if (!_hasEoz && _currentParseIndex < _source.size())
//	{
//		unsigned char ch = _source[_currentParseIndex];
//		return ch;
//	}
//	return EOZ;
//}
//
//void LuaDocTokenParser::ResetBuffer()
//{
//	_hasSaveText = false;
//	_buffStart = 0;
//	_buffIndex = 0;
//}
//
//std::string_view LuaDocTokenParser::GetSaveText()
//{
//	if (_hasSaveText)
//	{
//		return std::string_view(_source.data() + _buffStart, _buffIndex - _buffStart + 1);
//	}
//	else
//	{
//		return std::string_view(_source.data() + _buffStart, 0);
//	}
//}
