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
	PlaceholderElement,
	AlignIfLayoutElement,
	StringLiteralElement,
	CallArgsListLayoutElement,

	ControlStart,
	KeepLineElement,
	MinLineElement,
	MaxLineElement,
	KeepBlankElement,
	LineElement,
	AlignmentElement,
	AlignToElement,
	KeepElement,
	MaxSpaceElement
	
};