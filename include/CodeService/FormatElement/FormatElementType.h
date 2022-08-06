#pragma once

enum class FormatElementType
{
	FormatElement,
	IndentElement,
	StatementElement,
	TextElement,
	OperatorElement,
	KeyWordElement,
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
	NormalTableLayoutElement,

	ControlStart,
	KeepLineElement,
	MinLineElement,
	MaxLineElement,
	SpaceElement,
	LineElement,
	AlignmentElement,
	AlignToElement,
	KeepElement,
	MaxSpaceElement
	
};