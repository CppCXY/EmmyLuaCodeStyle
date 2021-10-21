#include "CodeService/FormatElement/AlignmentLayoutElement.h"
#include "CodeService/FormatElement/StatementElement.h"
#include "CodeService/FormatElement/TextElement.h"
#include "CodeService/FormatElement/AlignmentElement.h"

int FindLastValidChildIndex(int position, std::vector<std::shared_ptr<FormatElement>>& vec)
{
	for (int index = position - 1; position >= 0; index--)
	{
		if (vec[index]->HasValidTextRange())
		{
			return index;
		}
	}
	return -1;
}


FormatElementType AlignmentLayoutElement::GetType()
{
	return FormatElementType::AlignmentLayoutElement;
}

void AlignmentLayoutElement::Serialize(FormatContext& ctx, int position, FormatElement* parent)
{
	int eqAlignedPosition = 0;
	bool firstContainEq = true;
	// 先寻找等号对齐的位置，并且判断连续的带等号的语句是否应该对齐到等号
	// 连续的带等号的语句是否应该对齐到等号，这个行为应该由连续语句的首行决定
	// 如果被子节点内的其他语句共同决定则很难将连续对齐还原为普通排版
	for (int statIndex = 0; statIndex != _children.size(); statIndex++)
	{
		auto statChild = _children[statIndex];

		auto& statChildren = statChild->GetChildren();

		for (int index = 0; index != statChildren.size(); index++)
		{
			auto textChild = statChildren[index];
			if (textChild->GetType() == FormatElementType::TextElement)
			{
				auto textElement = std::dynamic_pointer_cast<TextElement>(textChild);
				if (textElement->GetText() == "=")
				{
					int eqPosition = ctx.GetColumn(textElement->GetTextRange().StartOffset);
					if (firstContainEq && index > 0)
					{
						firstContainEq = false;
						auto lastValidIndex = FindLastValidChildIndex(index, statChildren);
						if (lastValidIndex == -1)
						{
							return normalSerialize(ctx, position, this);
						}
						auto lastStatChild = statChildren[lastValidIndex];
						auto lastPosition = ctx.GetColumn(lastStatChild->GetTextRange().EndOffset);


						if (eqPosition - lastPosition <= 2)
						{
							return normalSerialize(ctx, position, this);
						}
					}

					if (eqAlignedPosition < eqPosition)
					{
						eqAlignedPosition = eqPosition;
					}
				}
			}
		}
	}

	return alignmentSerialize(ctx, position, this, eqAlignedPosition);
}

void AlignmentLayoutElement::alignmentSerialize(FormatContext& ctx, int position, FormatElement* parent, int eqPosition)
{
	for (auto statChild : _children)
	{
		auto& statChildren = statChild->GetChildren();

		for (auto it = statChildren.begin(); it != statChildren.end(); it++)
		{
			auto textChild = *it;
			if (textChild->GetType() == FormatElementType::TextElement)
			{
				auto textElement = std::dynamic_pointer_cast<TextElement>(textChild);
				if (textElement->GetText() == "=")
				{
					// 遍历时插入会导致迭代器失效，但我只会插入一次，所以该迭代器不会再使用
					statChildren.insert(it, std::make_shared<AlignmentElement>(eqPosition));
					break;
				}
			}
		}
	}

	return normalSerialize(ctx, position, parent);
}

void AlignmentLayoutElement::normalSerialize(FormatContext& ctx, int position, FormatElement* parent)
{
	FormatElement::Serialize(ctx, position, parent);
}
