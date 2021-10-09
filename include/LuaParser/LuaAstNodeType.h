#pragma once

enum class LuaAstNodeType
{
	Chunk,

	Block,

	Identify,

	KeyWord,

	LocalStatement,

	IfStatement,


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

	Param

};