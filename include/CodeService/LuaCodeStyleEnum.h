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
	RemoveStringLiteralOnly,
	RemoveTableOnly
};

