#include "CodeService/FormatElement/AlignIfLayoutElement.h"
#include "CodeService/FormatElement/TextElement.h"
#include "CodeService/FormatElement/KeepBlankElement.h"

AlignIfElement::AlignIfElement()
{
}

FormatElementType AlignIfElement::GetType()
{
	return FormatElementType::AlignIfLayoutElement;
}

void AlignIfElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	AlignElement(ctx);

	FormatElement::Serialize(ctx, selfIt, parent);
}

void AlignIfElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	AlignElement(ctx);

	FormatElement::Diagnosis(ctx, selfIt, parent);
}

void AlignIfElement::AlignElement(FormatContext& ctx)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		auto child = *it;
		if (child->GetType() == FormatElementType::TextElement)
		{
			auto textElement = std::dynamic_pointer_cast<TextElement>(child);
			if (textElement->GetText() == "if")
			{
				++it;
				if (it != _children.end())
				{
					*it = std::make_shared<KeepBlankElement>(5);
					continue;
				}
			}
		}

		if (child->GetType() == FormatElementType::LongExpressionLayoutElement)
		{
			AlignConditionExpression(ctx, *child);
		}
	}
}

void AlignIfElement::AlignConditionExpression(FormatContext& ctx, FormatElement& parent)
{
	auto& children = parent.GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		auto child = *it;

		if (child->GetType() == FormatElementType::SubExpressionElement)
		{
			AlignConditionExpression(ctx, *child);
		}
		else if (child->GetType() == FormatElementType::TextElement)
		{
			auto textElement = std::dynamic_pointer_cast<TextElement>(child);
			auto text = textElement->GetText();
			if (text == "and" || text == "or")
			{
				if (ctx.OnlyEmptyCharBefore(textElement->GetTextRange().StartOffset))
				{
					++it;
					if (it != children.end())
					{
						*it = std::make_shared<KeepBlankElement>(
							text == "and" ? 4 : 5
						);
					}
				}
			}
		}
	}
}
