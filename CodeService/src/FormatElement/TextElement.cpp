#include "CodeService/FormatElement/TextElement.h"
#include "Util/format.h"

TextElement::TextElement(std::string_view text, TextDefineType textDefineType, TextRange range)
	: FormatElement(range),
	  _text(text),
	  _textDefineType(textDefineType)
{
}

TextElement::TextElement(std::shared_ptr<LuaAstNode> node, TextDefineType textDefineType)
	: TextElement(node->GetText(), textDefineType, node->GetTextRange())
{
}

FormatElementType TextElement::GetType()
{
	return FormatElementType::TextElement;
}

void TextElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	ctx.Print(*this);
}

void TextElement::Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent)
{
	int character = ctx.GetColumn(_textRange.EndOffset);
	ctx.SetCharacterCount(character);

	if (character > ctx.GetOptions().max_line_length)
	{
		auto line = ctx.GetLine(_textRange.EndOffset);
		ctx.SetLineMaxLength(line, character);
	}


	if (ctx.GetOptions().enable_name_style_check)
	{
		switch (_textDefineType)
		{
		case TextDefineType::LocalNameDefine:
			{
				if (!NameStyleCheck(ctx.GetOptions().local_name_define_style))
				{
					ctx.PushDiagnosis(format(
						                  LText("local name define does not match {} name style"),
						                  TranslateNameStyle(ctx.GetOptions().local_name_define_style)), _textRange);
				}
				break;
			}
		case TextDefineType::FunctionNameDefine:
			{
				if (!NameStyleCheck(ctx.GetOptions().function_name_define_style))
				{
					ctx.PushDiagnosis(format(
						                  LText("function name define does not match {} name style"),
						                  TranslateNameStyle(ctx.GetOptions().function_name_define_style)), _textRange);
				}
				break;
			}
		case TextDefineType::TableFieldNameDefine:
			{
				if (!NameStyleCheck(ctx.GetOptions().table_field_name_define_style))
				{
					ctx.PushDiagnosis(format(
						                  LText("table field name define does not match {} name style"),
						                  TranslateNameStyle(ctx.GetOptions().table_field_name_define_style)),
					                  _textRange);
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}
}

std::string_view TextElement::GetText() const
{
	return _text;
}

void TextElement::SetTextDefineType(TextDefineType textDefineType)
{
	_textDefineType = textDefineType;
}

std::string TextElement::TranslateNameStyle(NameStyle style)
{
	switch (style)
	{
	case NameStyle::CamelCase:
		{
			return "camel-case";
		}
	case NameStyle::PascalCase:
		{
			return "pascal-case";
		}
	case NameStyle::SnakeCase:
		{
			return "snake-case";
		}
	default:
		{
			return "unknwon";
		}
	}
}

bool TextElement::NameStyleCheck(NameStyle style)
{
	switch (style)
	{
	case NameStyle::SnakeCase:
		{
			return SnakeCase();
		}
	case NameStyle::PascalCase:
		{
			return PascalCase();
		}
	case NameStyle::CamelCase:
		{
			return CamelCase();
		}
	default:
		{
			break;
		}
	}
	return true;
}

bool TextElement::SnakeCase()
{
	bool lowerCase = false;
	for (std::size_t index = 0; index != _text.size(); index++)
	{
		char ch = _text[index];

		if (index == 0)
		{
			if (::islower(ch))
			{
				lowerCase = true;
			}
			else if (::isupper(ch))
			{
				lowerCase = false;
			}
			else if(ch == '_' && _text.size() == 1)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if ((lowerCase && !::islower(ch)) || (!lowerCase && !::isupper(ch)))
		{

			if (ch == '_')
			{
				// 不允许双下划线
				if ((index < _text.size() - 1) && _text[index + 1] == '_')
				{
					return false;
				}
			}
			else if(!::isdigit(ch))
			{
				return false;
			}
		}
	}
	return true;
}

bool TextElement::PascalCase()
{
	for (std::size_t index = 0; index != _text.size(); index++)
	{
		char ch = _text[index];

		if (index == 0)
		{
			// 首字母必须大写
			if (!::isupper(ch))
			{
				// _ 亚元不受命名限制
				if (_text.size() == 1 && ch == '_')
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		// 我又没办法分词简单处理下
		else if (!::isalnum(ch))
		{
			return false;
		}
	}
	return true;
}

bool TextElement::CamelCase()
{
	for (std::size_t index = 0; index != _text.size(); index++)
	{
		char ch = _text[index];

		if (index == 0)
		{
			// 首字母可以小写，也可以单下划线开头
			if (!::islower(ch))
			{
				if (ch == '_')
				{
					if (_text.size() > 1 && !::islower(ch))
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
		}
		else if (!::isalnum(ch))
		{
			return false;
		}
	}
	return true;
}
