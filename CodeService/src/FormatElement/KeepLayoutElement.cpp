#include "CodeService/FormatElement/KeepLayoutElement.h"

FormatElementType KeepLayoutElement::GetType()
{
	return FormatElementType::KeepLayoutElement;
}

void KeepLayoutElement::Serialize(FormatContext& ctx, int position, FormatElement* parent)
{
	// 因为会向前追溯，所以使用int
	for (int i = 0; i != _children.size(); i++)
	{
		auto child = _children[i];
		if(static_cast<int>(child->GetType()) < static_cast<int>(FormatElementType::ControlStart))
		{
			
		}


		_children[i]->Serialize(ctx, i, this);
	}
}
