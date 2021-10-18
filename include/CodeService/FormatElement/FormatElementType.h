#pragma once

enum class FormatElementType
{
	Normal,
	IndentElement,
	StatementElement,
	TextElement,
	ExpressionElement,

	ControlStart,
	KeepLineElement,
	MinLineElement,
	KeepBlankElement,
	LineElement,
};