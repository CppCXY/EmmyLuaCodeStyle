#pragma once

enum class LuaAstNodeType
{
	Chunk,

	Block,

	Identify,

	KeyWord,

	EmptyStatement,

	LocalStatement,

	IfStatement,

	WhileStatement,

	DoStatement,

	Expression,
	LiteralExpression,
	ClosureExpression,
	UnaryExpression,
	BinaryExpression,
	TableExpression,
	CallExpression,
	IndexExpression,

	UnaryOperator,

	BinaryOperator,

	IndexOperator,

	TableField,

	FunctionBody,

	ParamList,

	Param,

	CallArgList,

	ExpressionList
};