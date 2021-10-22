#include "CodeService/FormatElement/LongExpressionLayoutElement.h"

FormatElementType LongExpressionLayoutElement::GetType()
{
	return FormatElementType::KeepLayoutElement;
}

void LongExpressionLayoutElement::Serialize(FormatContext& ctx, int position, FormatElement* parent)
{

	for (int i = 0; i != _children.size(); i++)
	{
		auto child = _children[i];
		if(child->GetType()  == FormatElementType::ControlStart)
		{
			
		}


		_children[i]->Serialize(ctx, i, this);
	}
}
