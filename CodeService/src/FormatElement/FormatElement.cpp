#include "CodeService/FormatElement/FormatElement.h"
#include "Util/format.h"

FormatElement::FormatElement(TextRange range)
	: _textRange(range)
{
}

FormatElement::~FormatElement()
{
}

FormatElementType FormatElement::GetType()
{
	return FormatElementType::FormatElement;
}

TextRange FormatElement::GetTextRange()
{
	return _textRange;
}


void FormatElement::AddChild(std::shared_ptr<FormatElement> child)
{
	if (!child->GetTextRange().IsEmpty())
	{
		const auto range = child->GetTextRange();
		AddTextRange(range);
	}
	_children.push_back(child);
}

void FormatElement::AddChildren(ChildContainer& children)
{
	for (const auto& child : children)
	{
		AddChild(child);
	}
}

FormatElement::ChildContainer& FormatElement::GetChildren()
{
	return _children;
}

bool FormatElement::HasValidTextRange() const
{
	return !_textRange.IsEmpty();
}

std::shared_ptr<FormatElement> FormatElement::LastValidElement() const
{
	for (auto it = _children.rbegin(); it != _children.rend(); ++it)
	{
		if (static_cast<int>((*it)->GetType()) < static_cast<int>(FormatElementType::ControlStart))
		{
			return *it;
		}
	}

	return nullptr;
}

void FormatElement::Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		(*it)->Serialize(ctx, it, *this);
	}
}

void FormatElement::Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		(*it)->Diagnosis(ctx, it, *this);
	}
}

void FormatElement::Format(SerializeContext& ctx)
{
	// workaround
	auto root = std::make_shared<FormatElement>();
	root->AddChild(shared_from_this());
	auto it = root->GetChildren().begin();
	Serialize(ctx, it, *root);
}

void FormatElement::DiagnosisCodeStyle(DiagnosisContext& ctx)
{
	// workaround
	auto root = std::make_shared<FormatElement>();
	root->AddChild(shared_from_this());
	auto it = root->GetChildren().begin();
	Diagnosis(ctx, it, *root);
}

void FormatElement::AddTextRange(TextRange range)
{
	if (_textRange.IsEmpty())
	{
		_textRange = range;
	}

	if (range.StartOffset < _textRange.StartOffset)
	{
		_textRange.StartOffset = range.StartOffset;
	}

	if (range.EndOffset > _textRange.EndOffset)
	{
		_textRange.EndOffset = range.EndOffset;
	}
}


int FormatElement::GetLastValidLine(FormatContext& ctx, ChildIterator it, FormatElement& parent)
{
	return ctx.GetLine(GetLastValidOffset(it, parent));
}

int FormatElement::GetNextValidLine(FormatContext& ctx, ChildIterator it, FormatElement& parent)
{
	const auto offset = GetNextValidOffset(it, parent);
	if (offset != -1)
	{
		return ctx.GetLine(offset);
	}
	else
	{
		return -1;
	}
}

void FormatElement::GeneralIndentDiagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		const auto child = *it;

		if (child->HasValidTextRange()
			&& child->GetType() != FormatElementType::IndentElement
			&& child->GetType() != FormatElementType::NoIndentElement)
		{
			auto writeCharacter = ctx.GetCharacterCount();
			if (writeCharacter != 0)
			{
				goto endIndentDiagnose;
			}

			auto range = child->GetTextRange();
			auto line = ctx.GetLine(range.StartOffset);
			auto character = ctx.GetColumn(range.StartOffset);
			auto indentState = ctx.CalculateIndentState(range.StartOffset);
			auto state = ctx.GetCurrentIndent();
			if (ctx.GetOptions().indent_style != indentState.Style)
			{
				ctx.PushDiagnosis(
					format(LText("incorrect indentation style, expect {}, but here is {}"),
					       GetIndentStyleName(state.Style),
					       GetIndentStyleName(indentState.Style)
					),
					LuaDiagnosisPosition(line, 0),
					LuaDiagnosisPosition(line, character)
				);
				goto endIndentDiagnose;
			}

			if (indentState.Style == IndentStyle::Space)
			{
				if (state.SpaceIndent != indentState.SpaceIndent)
				{
					ctx.PushDiagnosis(
						format(LText("incorrect indentation {}, here need {} space indentation"),
						       indentState.SpaceIndent, state.SpaceIndent),
						LuaDiagnosisPosition(line, 0),
						LuaDiagnosisPosition(line, character)
					);
				}
				goto endIndentDiagnose;
			}
			else
			{
				if (state.SpaceIndent != indentState.SpaceIndent || state.TabIndent != indentState.TabIndent)
				{
					ctx.PushDiagnosis(
						format(LText("incorrect indentation, here need {} tab and {} space indentation"),
						       state.TabIndent, state.SpaceIndent),
						LuaDiagnosisPosition(line, 0),
						LuaDiagnosisPosition(line, character)
					);
				}
			}
		}

	endIndentDiagnose:
		child->Diagnosis(ctx, it, *this);
	}
}

void FormatElement::CopyFrom(std::shared_ptr<FormatElement> node)
{
	_textRange = node->_textRange;
	_children = node->_children;
}

void FormatElement::Reset()
{
	_textRange = TextRange();
	_children.clear();
}

int FormatElement::GetLastValidOffset(ChildIterator& it, FormatElement& parent)
{
	auto& siblings = parent.GetChildren();
	auto rIt = std::reverse_iterator<std::remove_reference_t<decltype(it)>>(it);

	for (; rIt != siblings.rend(); ++rIt)
	{
		auto sibling = *rIt;
		if (sibling->HasValidTextRange())
		{
			return sibling->GetTextRange().EndOffset;
		}
	}

	// 那么一定是往上找不到有效范围元素
	return parent.GetTextRange().StartOffset;
}

int FormatElement::GetNextValidOffset(ChildIterator& it, FormatElement& parent)
{
	auto& siblings = parent.GetChildren();
	++it;
	for (; it != siblings.end(); ++it)
	{
		auto child = *it;
		if (child->HasValidTextRange())
		{
			return child->GetTextRange().StartOffset;
		}
	}

	return -1;
}
