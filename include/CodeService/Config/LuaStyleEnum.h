#pragma once

enum class IndentStyle
{
	Tab,
	Space,
};

enum class QuoteStyle
{
	None,
	Single,
	Double
};

enum class CallArgParentheses : int
{
	Keep,
	Remove,
	RemoveStringOnly,
	RemoveTableOnly,
	UnambiguousRemoveStringOnly
};

enum class AlignCallArgs
{
	True,
	False,
	OnlyAfterMoreIndentionStatement,
	OnlyNotExistCrossExpression,
};

enum class TableSeparatorStyle
{
	None,
	Comma,
	Semicolon
};

enum class TrailingTableSeparator
{
	Keep,
	Never,
	Smart,
	Always
};
