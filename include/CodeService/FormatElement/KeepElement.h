﻿#pragma once

#include "KeepLineElement.h"

class KeepElement : public FormatElement
{
public:
	/*
	 * @param keepBlank 表示如果下一个元素和当前元素在同一行则保持几个空格
	 * @remark 如果下一个元素和当前元素在不同行则换行
	 */
	explicit KeepElement(int keepBlank, bool hasLinebreak = false);

	FormatElementType GetType() override;

	void Serialize(SerializeContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
	void Diagnose(DiagnosisContext& ctx, ChildIterator selfIt, FormatElement& parent) override;
private:
	int _keepBlank;
	bool _hasLinebreak;
};
