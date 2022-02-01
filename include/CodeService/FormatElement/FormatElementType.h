#pragma once

enum class FormatElementType
{
	FormatElement,
	IndentElement,
	StatementElement,
	TextElement,
	ExpressionElement,
	AlignmentLayoutElement,
	KeepLayoutElement,
	AlignToFirstElement,
	SubExpressionElement,
	LongExpressionLayoutElement,
	NoIndentElement,
	IndentOnLineBreakElement,

	ControlStart,
	KeepLineElement,
	MinLineElement,
	KeepBlankElement,
	LineElement,
	AlignmentElement,
	AlignToElement,
	KeepElement,
	
};