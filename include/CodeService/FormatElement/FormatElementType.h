#pragma once

enum class FormatElementType
{
	Normal,
	IndentElement,
	StatementElement,
	TextElement,
	ExpressionElement,
	AlignmentLayoutElement,
	KeepLayoutElement,
	AlignToFirstElement,
	SubExpressionElement,
	LongExpressionLayoutElement,

	ControlStart,
	KeepLineElement,
	MinLineElement,
	KeepBlankElement,
	LineElement,
	AlignmentElement,
	AlignToElement,
	KeepElement,
};