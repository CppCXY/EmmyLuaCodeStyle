#include "CodeService/FormatElement/AlignmentLayoutElement.h"
#include "CodeService/FormatElement/StatementElement.h"
#include "CodeService/FormatElement/TextElement.h"
#include "CodeService/FormatElement/AlignmentElement.h"

std::shared_ptr<FormatElement> FindLastValidChild(FormatElement::ChildIterator it,
                                                  FormatElement::ChildContainer& children)
{
	for (auto rIt = std::reverse_iterator<decltype(it)>(it); rIt != children.rend(); ++rIt)
	{
		if ((*rIt)->HasValidTextRange())
		{
			return *rIt;
		}
	}
	return nullptr;
}

AlignmentLayoutElement::AlignmentLayoutElement(LuaTokenType alignToken)
	: _alignToken(alignToken)
{
}

FormatElementType AlignmentLayoutElement::GetType()
{
	return FormatElementType::AlignmentLayoutElement;
}

void AlignmentLayoutElement::Serialize(SerializeContext& ctx, ChildIterator selfIt,
                                       FormatElement& parent)
{
	const auto alignSignPosition = GetAlignPosition(ctx);

	if (alignSignPosition != -1)
	{
		return AlignmentSerialize(ctx, selfIt, alignSignPosition, parent);
	}
	else
	{
		FormatElement::Serialize(ctx, selfIt, parent);
	}
}

void AlignmentLayoutElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt,
                                       FormatElement& parent)
{
	const auto eqPosition = GetAlignPosition(ctx);

	if (eqPosition != -1)
	{
		return AlignmentDiagnosis(ctx, selfIt, eqPosition, parent);
	}
	else
	{
		FormatElement::Diagnosis(ctx, selfIt, parent);
	}
}

int AlignmentLayoutElement::GetAlignPosition(FormatContext& ctx)
{
	auto indentState = ctx.GetCurrentIndent();
	// if (!ctx.GetOptions().weak_alignment_rule)
	// {
	// 	alignOffset = GetAlignOffset(ctx);
	// }
	// else
	// {
	int alignOffset = GetAlignOffsetWithWeakRule(ctx);
	// }

	if (alignOffset == -1)
	{
		return alignOffset;
	}

	return alignOffset + static_cast<int>(indentState.SpaceIndent + indentState.TabIndent);
}

// int AlignmentLayoutElement::GetAlignOffset(FormatContext& ctx)
// {
// 	int alignSignOffset = 0;
// 	bool firstContainAlignSign = true;
// 	// 先寻找等号对齐的位置，并且判断连续的带等号的语句是否应该对齐到等号
// 	// 连续的带等号的语句是否应该对齐到等号，这个行为应该由连续语句的首行决定
// 	// 如果被子节点内的其他语句共同决定则很难将连续对齐还原为普通排版
// 	for (auto statIt = _children.begin(); statIt != _children.end(); ++statIt)
// 	{
// 		const auto statement = *statIt;
//
// 		auto& statementChildren = statement->GetChildren();
//
// 		for (auto it = statementChildren.begin(); it != statementChildren.end(); ++it)
// 		{
// 			auto textChild = *it;
// 			if (textChild->GetType() == FormatElementType::TextElement)
// 			{
// 				const auto textElement = std::dynamic_pointer_cast<TextElement>(textChild);
// 				if (textElement->GetText() == _alignSign)
// 				{
// 					const auto signPosition = ctx.GetColumn(textElement->GetTextRange().StartOffset);
// 					if (firstContainAlignSign && it != statementChildren.begin())
// 					{
// 						firstContainAlignSign = false;
// 						auto lastStatChild = FindLastValidChild(it, statementChildren);
// 						if (lastStatChild == nullptr)
// 						{
// 							return -1;
// 						}
//
// 						const auto lastPosition = ctx.GetColumn(lastStatChild->GetTextRange().EndOffset);
//
// 						if (signPosition - lastPosition <= 2)
// 						{
// 							return -1;
// 						}
// 					}
//
//
// 					alignSignOffset = std::max(alignSignOffset,
// 					                           signPosition - ctx.GetColumn(statement->GetTextRange().StartOffset)
// 					);
// 				}
// 			}
// 		}
// 	}
// 	return alignSignOffset;
// }

int AlignmentLayoutElement::GetAlignOffsetWithWeakRule(FormatContext& ctx)
{
	int alignSignOffset = 0;
	bool canAlignToSign = false;
	// 先寻找等号对齐的位置，并且判断连续的带等号的语句是否应该对齐到等号
	// 连续的带等号的语句是否应该对齐到等号，这个行为由所有连续语句共同决定
	for (auto statIt = _children.begin(); statIt != _children.end(); ++statIt)
	{
		const auto statement = *statIt;

		auto& statementChildren = statement->GetChildren();

		for (auto it = statementChildren.begin(); it != statementChildren.end(); ++it)
		{
			auto textChild = *it;
			if (textChild->GetType() == FormatElementType::TextElement)
			{
				const auto textElement = std::dynamic_pointer_cast<TextElement>(textChild);
				const auto textNode = textElement->GetNode();
				if (textNode && textNode->GetTokenType() == _alignToken)
				{
					const auto signPosition = ctx.GetColumn(textElement->GetTextRange().StartOffset);

					auto lastStatChild = FindLastValidChild(it, statementChildren);
					if (lastStatChild == nullptr)
					{
						return -1;
					}

					const auto lastPosition = ctx.GetColumn(lastStatChild->GetTextRange().EndOffset);

					if (signPosition - lastPosition > 2)
					{
						canAlignToSign = true;
					}
					// 采用最小对齐原则
					alignSignOffset = std::max(alignSignOffset,
					                           lastPosition + 2 - ctx.GetColumn(statement->GetTextRange().StartOffset)
					);
				}
			}
		}
	}

	return canAlignToSign ? alignSignOffset : -1;
}

void AlignmentLayoutElement::AlignmentSerialize(SerializeContext& ctx, ChildIterator selfIt, int alignSignPosition,
                                                FormatElement& parent)
{
	for (const auto& statChild : _children)
	{
		auto& statChildren = statChild->GetChildren();

		for (auto it = statChildren.begin(); it != statChildren.end(); ++it)
		{
			auto textChild = *it;
			if (textChild->GetType() == FormatElementType::TextElement)
			{
				const auto textElement = std::dynamic_pointer_cast<TextElement>(textChild);
				const auto textNode = textElement->GetNode();
				if (textNode && textNode->GetTokenType() == _alignToken && it != statChildren.begin())
				{
					auto lastIt = it;
					--lastIt;
					// 将控制元素变更为对齐元素
					*lastIt = std::make_shared<AlignmentElement>(alignSignPosition);
					break;
				}
			}
		}
	}

	return FormatElement::Serialize(ctx, selfIt, parent);
}

void AlignmentLayoutElement::AlignmentDiagnosis(DiagnosisContext& ctx,
                                                ChildIterator selfIt,
                                                int alignSignPosition, FormatElement& parent)
{
	for (const auto& statChild : _children)
	{
		auto& statChildren = statChild->GetChildren();

		for (auto it = statChildren.begin(); it != statChildren.end(); ++it)
		{
			auto textChild = *it;
			if (textChild->GetType() == FormatElementType::TextElement)
			{
				const auto textElement = std::dynamic_pointer_cast<TextElement>(textChild);
				const auto textNode = textElement->GetNode();
				if (textNode && textNode->GetTokenType() == _alignToken && it != statChildren.begin())
				{
					auto lastIt = it;
					--lastIt;
					// 将控制元素变更为对齐元素
					*lastIt = std::make_shared<AlignmentElement>(alignSignPosition);
					break;
				}
			}
		}
	}

	return FormatElement::Diagnosis(ctx, selfIt, parent);
}
