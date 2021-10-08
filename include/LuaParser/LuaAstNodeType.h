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

	UnaryOperator,

	BinaryOperator


};