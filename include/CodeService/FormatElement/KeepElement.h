#pragma once

#include "KeepLineElement.h"

class KeepElement : public KeepLineElement
{
public:
	/*
	 * @param keepBlank 表示如果下一个元素和当前元素在同一行则保持几个空格
	 * @remark 如果下一个元素和当前元素在不同行则换行
	 */
	KeepElement(int keepBlank);

	FormatElementType GetType() override;

	void Serialize(FormatContext& ctx, int position, FormatElement* parent) override;
private:
	int _keepBlank;
};
