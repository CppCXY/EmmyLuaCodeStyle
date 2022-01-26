#include "CodeService/FormatElement/AlignToFirstElement.h"

AlignToFirstElement::AlignToFirstElement()
{
}

FormatElementType AlignToFirstElement::GetType()
{
	return FormatElementType::AlignToFirstElement;
}

void AlignToFirstElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		const auto child = *it;
		if (it == _children.begin())
		{
			auto writeCount = ctx.GetCharacterCount();
			auto indentState = ctx.GetCurrentIndent();
			switch (indentState.IndentStyle)
			{
			case IndentStyle::Space:
				{
					if (writeCount > indentState.SpaceIndent)
					{
						indentState.SpaceIndent = writeCount;
					}
					else
					{
						indentState.SpaceIndent = ctx.GetColumn(child->GetTextRange().StartOffset);
					}
					break;
				}
			case IndentStyle::Tab:
				{
					if (writeCount > indentState.TabIndent)
					{
						indentState.SpaceIndent += writeCount - indentState.TabIndent;
					}
					else
					{
						auto& options = ctx.GetOptions();
						auto state = ctx.CalculateIndentState(child->GetTextRange().StartOffset);
						if (state.TabIndent < indentState.TabIndent)
						{
							auto diff = (indentState.TabIndent - state.TabIndent) * options.tab_width;
							if (state.SpaceIndent > diff)
							{
								indentState.SpaceIndent = state.SpaceIndent - diff;
							}
						}
						else
						{
							indentState.SpaceIndent = (state.TabIndent - indentState.TabIndent) * options.tab_width +
								state.SpaceIndent;
						}
					}
					break;
				}
			}
			ctx.AddIndent(indentState);
		}

		child->Serialize(ctx, it, *this);
	}

	if (!_children.empty())
	{
		ctx.RecoverIndent();
	}
}

void AlignToFirstElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		const auto child = *it;
		if (it == _children.begin())
		{
			auto writeCount = ctx.GetCharacterCount();
			auto indentState = ctx.GetCurrentIndent();
			switch (indentState.IndentStyle)
			{
			case IndentStyle::Space:
			{
				if (writeCount > indentState.SpaceIndent)
				{
					indentState.SpaceIndent = writeCount;
				}
				else
				{
					indentState.SpaceIndent = ctx.GetColumn(child->GetTextRange().StartOffset);
				}
				break;
			}
			case IndentStyle::Tab:
			{
				if (writeCount > indentState.TabIndent)
				{
					indentState.SpaceIndent += writeCount - indentState.TabIndent;
				}
				else
				{
					auto& options = ctx.GetOptions();
					auto state = ctx.CalculateIndentState(child->GetTextRange().StartOffset);
					if (state.TabIndent < indentState.TabIndent)
					{
						auto diff = (indentState.TabIndent - state.TabIndent) * options.tab_width;
						if (state.SpaceIndent > diff)
						{
							indentState.SpaceIndent = state.SpaceIndent - diff;
						}
					}
					else
					{
						indentState.SpaceIndent = (state.TabIndent - indentState.TabIndent) * options.tab_width +
							state.SpaceIndent;
					}
				}
				break;
			}
			}
			ctx.AddIndent(indentState);
		}

		child->Diagnosis(ctx, it, *this);
	}

	if (!_children.empty())
	{
		ctx.RecoverIndent();
	}
}
