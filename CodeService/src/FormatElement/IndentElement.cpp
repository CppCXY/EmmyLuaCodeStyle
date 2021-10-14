#include "CodeService/FormatElement/IndentElement.h"
#include "CodeService/FormatElement/KeepLineElement.h"
#include "CodeService/FormatElement/MinLineElement.h"
#include "CodeService/FormatElement/TextElement.h"

IndentElement::IndentElement()
	: FormatElement(TextRange(0,0))
{
}

FormatElementType IndentElement::GetType()
{
	return FormatElementType::IndentElement;
}

void IndentElement::AddChild(std::shared_ptr<FormatElement> child)
{
	_children.push_back(child);
	return FormatElement::AddChild(child);
}

void IndentElement::AddChild(std::shared_ptr<LuaAstNode> node)
{
	_children.push_back(std::make_shared<TextElement>(node->GetText(), node->GetTextRange()));
	return FormatElement::AddChild(node);
}

void IndentElement::KeepLine(int line)
{
	AddChild(std::make_shared<KeepLineElement>(TextRange(_textRange.EndOffset,_textRange.EndOffset), line));
}

void IndentElement::MinLine(int line)
{
	AddChild(std::make_shared<MinLineElement>(TextRange(_textRange.EndOffset, _textRange.EndOffset), line));
}

void IndentElement::Serialize(FormatContext& ctx)
{
	ctx.AddIndent();

	for (int index = 0; index != _children.size(); index++)
	{
		auto element = _children[index];
		switch (element->GetType())
		{
		case FormatElementType::KeepLineElement:
			{
				auto keepLineElement = std::dynamic_pointer_cast<KeepLineElement>(element);
				if (keepLineElement->Line == -1)
				{
					int firstElementLine = 0;
					int secondElementLine = 0;
					if (index == 0)
					{
						firstElementLine = ctx.GetLine(GetTextRange().StartOffset);
					}
					else
					{
						firstElementLine = ctx.GetLine(_children[index - 1]->GetTextRange().EndOffset);
					}

					if (index == _children.size() - 1)
					{
						secondElementLine = ctx.GetLine(GetTextRange().EndOffset);
					}
					else
					{
						secondElementLine = ctx.GetLine(_children[index + 1]->GetTextRange().StartOffset);
					}

					int line = secondElementLine - firstElementLine;
					if (line <= 0)
					{
						line = 1;
					}

					ctx.PrintLine(line);
				}
				else
				{
					ctx.PrintLine(keepLineElement->Line);
				}
				break;
			}
		case FormatElementType::MinLineElement:
			{
				auto minLineElement = std::dynamic_pointer_cast<MinLineElement>(element);
				auto minLine = 0;
				if (minLineElement->Line > 0)
				{
					minLine = minLineElement->Line;
				}

				int firstElementLine = 0;
				int secondElementLine = 0;
				if (index == 0)
				{
					firstElementLine = ctx.GetLine(GetTextRange().StartOffset);
				}
				else
				{
					firstElementLine = ctx.GetLine(_children[index - 1]->GetTextRange().EndOffset);
				}

				if (index == _children.size() - 1)
				{
					secondElementLine = ctx.GetLine(GetTextRange().EndOffset);
				}
				else
				{
					secondElementLine = ctx.GetLine(_children[index + 1]->GetTextRange().StartOffset);
				}

				int line = secondElementLine - firstElementLine;
				if (line <= minLine)
				{
					line = minLine;
				}

				ctx.PrintLine(minLine);
				break;
			}
		default:
			{
				element->Serialize(ctx);
				break;
			}
		}
	}
	ctx.RecoverIndent();
}
