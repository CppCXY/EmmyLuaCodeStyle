#pragma once

enum class LuaAstNodeType
{
	Chunk,

	Block,

	Identify,

	NameIdentify,

	KeyWord,

	EmptyStatement,

	LocalStatement,

	LocalFunctionStatement,

	IfStatement,

	WhileStatement,

	DoStatement,

	ForStatement,

	RepeatStatement,

	FunctionStatement,

	LabelStatement,

	BreakStatement,

	ReturnStatement,

	GotoStatement,

	ExpressionStatement,

	AssignStatement,

	Expression,
	PrimaryExpression,
	LiteralExpression,
	ClosureExpression,
	UnaryExpression,
	BinaryExpression,
	TableExpression,
	CallExpression,
	IndexExpression,
	NameExpression,

	UnaryOperator,

	BinaryOperator,

	IndexOperator,

	GeneralOperator,

	TableField,

	TableFieldSep,

	FunctionBody,

	ParamList,

	NameDefList,

	Attribute,

	Param,

	CallArgList,

	ExpressionList,

	ForNumber,

	ForList,

	ForBody,

	Error,

	Comment,

	ShortComment,

	LongComment,

	ShebangComment,
};