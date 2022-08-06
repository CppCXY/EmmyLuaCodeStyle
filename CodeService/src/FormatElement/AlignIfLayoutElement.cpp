#include "CodeService/FormatElement/AlignIfLayoutElement.h"

#include "CodeService/FormatElement/KeyWordElement.h"
#include "CodeService/FormatElement/OperatorElement.h"
#include "CodeService/FormatElement/TextElement.h"
#include "CodeService/FormatElement/SpaceElement.h"

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
	// WeakDiagnosis(ctx, selfIt, parent);
}

bool AlignIfElement::ElseifFounded()
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		auto child = *it;
		if (child->Is(FormatElementType::KeyWordElement))
		{
			auto keyElement = std::dynamic_pointer_cast<KeyWordElement>(child);
			if (keyElement->GetText() == "elseif")
			{
				return true;
			}
		}
	}
	return false;
}

bool AlignIfElement::OnlyEmptyCharBeforeAnd(FormatContext& ctx, FormatElement& expressionLayout)
{
	auto& children = expressionLayout.GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		auto child = *it;

		if (child->GetType() == FormatElementType::SubExpressionElement)
		{
			if (OnlyEmptyCharBeforeAnd(ctx, *child))
			{
				return true;
			}
		}
		else if (child->Is(FormatElementType::OperatorElement))
		{
			auto opElement = std::dynamic_pointer_cast<OperatorElement>(child);
			auto text = opElement->GetText();
			if (text == "and")
			{
				if (ctx.OnlyEmptyCharBefore(opElement->GetTextRange().StartOffset))
				{
					return true;
				}
			}
		}
	}

	return false;
}

void AlignIfElement::AlignElement(FormatContext& ctx)
{
	bool elseIfFounded = ElseifFounded();
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		auto child = *it;
		if (child->Is(FormatElementType::KeyWordElement))
		{
			auto textElement = std::dynamic_pointer_cast<KeyWordElement>(child);
			if (textElement->GetText() == "if")
			{
				++it;
				auto controlIt = it;
				if (controlIt == _children.end())
				{
					continue;
				}

				++it;
				if (it != _children.end()
					&& (*it)->GetType() == FormatElementType::LongExpressionLayoutElement
					&& (*controlIt)->GetType() == FormatElementType::SpaceElement)
				{
					auto expressionLayout = *it;
					if (elseIfFounded)
					{
						int ifOffset = child->GetTextRange().EndOffset;
						int expressionOffset = expressionLayout->GetTextRange().StartOffset;

						int spaceAfterIf = expressionOffset - ifOffset - 1;
						if(spaceAfterIf == 2 || spaceAfterIf == 1)
						{
							*controlIt = std::make_shared<SpaceElement>(2);
							AlignConditionExpression(ctx, *expressionLayout, 4);
						}
						else {
							// if(expressionOffset - ifOffset >= )
							// if<5 space>condition
							// elseif<1 space>condition
							*controlIt = std::make_shared<SpaceElement>(5);
							AlignConditionExpression(ctx, *expressionLayout, 7);
						}
					}
					else if (OnlyEmptyCharBeforeAnd(ctx, *expressionLayout))
					{
						// if<2 space>condition
						// and<1 space>condition
						*controlIt = std::make_shared<SpaceElement>(2);
						AlignConditionExpression(ctx, *expressionLayout, 4);
					}
				}
			}
		}

		if (child->GetType() == FormatElementType::LongExpressionLayoutElement)
		{
			AlignConditionExpression(ctx, *child, 7);
		}
	}
}

void AlignIfElement::AlignConditionExpression(FormatContext& ctx, FormatElement& parent,
                                              int spacePositionAfterIndent)
{
	auto& children = parent.GetChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		auto child = *it;

		if (child->GetType() == FormatElementType::SubExpressionElement)
		{
			AlignConditionExpression(ctx, *child, spacePositionAfterIndent);
		}
		else if (child->Is(FormatElementType::OperatorElement))
		{
			auto opElement = std::dynamic_pointer_cast<OperatorElement>(child);
			auto text = opElement->GetText();
			if (text == "and" || text == "or")
			{
				if (ctx.OnlyEmptyCharBefore(opElement->GetTextRange().StartOffset))
				{
					++it;
					if (it != children.end())
					{
						*it = std::make_shared<SpaceElement>(
							static_cast<int>(spacePositionAfterIndent - text.size())
						);
					}
				}
			}
		}
	}
}

