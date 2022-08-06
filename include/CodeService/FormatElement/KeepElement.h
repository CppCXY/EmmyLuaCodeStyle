#pragma once

#include "SpaceElement.h"
#include "BreakableElement.h"

class KeepElement : public SpaceElement
{
public:

	/*
	 * @param keepSpace 表示如果下一个元素和当前元素在同一行则保持几个空格
	 * @remark 如果下一个元素和当前元素在不同行则换行
	 */
	explicit KeepElement(int keepSpace, bool hasLinebreak = false, bool allowContinueIndent = true);

	FormatElementType GetType() override;

	bool AllowContinueIndent() const { return _allowContinueIndent; }

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnosis(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;

	void AllowBreakLineSerialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent);
private:
	bool _hasLinebreak;
	bool _allowContinueIndent;
};

