#include "CodeService/FormatElement/SepElement.h"

SepElement::SepElement()
	: TextElement(""),
	  _empty(false)
{
}

SepElement::SepElement(std::shared_ptr<LuaAstNode> node)
	: TextElement(node),
	  _empty(false)
{
}

FormatElementType SepElement::GetType()
{
	return FormatElementType::SepElement;
}

void SepElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	if (_empty)
	{
		_text = "";
	}
	else
	{
		switch (ctx.GetOptions().table_separator_style)
		{
		case TableSeparatorStyle::Comma:
			{
				_text = ",";
				break;
			}
		case TableSeparatorStyle::Semicolon:
			{
				_text = ";";
				break;
			}
		default:
			{
				if(_text.empty())
				{
					_text = ",";
				}

				break;
			}
		}
	}

	TextElement::Serialize(ctx, selfIt, parent);
}

void SepElement::SetEmpty()
{
	_empty = true;
}
