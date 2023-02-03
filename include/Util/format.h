#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <sstream>
#include <type_traits>

// 加命名空间是为了不和std format混淆
namespace util {

class FormatReplaceExpress
{
public:
	FormatReplaceExpress(std::string_view expr, std::size_t startIndex, std::size_t endIndex, bool needReplace)
		: Expr(expr),
		  StartIndex(startIndex),
		  EndIndex(endIndex),
		  NeedReplace(needReplace)
	{
	}

	std::string_view Expr;
	std::size_t StartIndex;
	std::size_t EndIndex;
	bool NeedReplace;
};

#if __cplusplus > 202002L || _MSVC_LANG > 202002L
template <class T>
std::string toFormatString(T t)
{
	if constexpr (std::is_same_v<T, std::string_view>)
	{
		return std::string(t);
	}
	else if constexpr (std::is_same_v<T, std::string>)
	{
		return t;
	}
	else if constexpr (std::is_same_v<T, const char*>)
	{
		return std::string(t);
	}
	else
	{
		return std::to_string(t);
	}
}
#else

template <class T>
inline std::string toFormatString(T t)
{
	return std::to_string(t);
}

template <>
inline std::string toFormatString(std::string t)
{
	return t;
}

template <>
inline std::string toFormatString(std::string_view t)
{
	return std::string(t);
}

template <>
inline std::string toFormatString(const char* t)
{
	return std::string(t);
}

#endif


/*
 * 直到该项目发起时,gcc依然没有完成C++20 format 实现
 * 自己写一个
 */
template <class... ARGS>
std::string format(std::string_view fmt, ARGS ... args)
{
	std::vector<std::string> argVec = {toFormatString<ARGS>(args)...};

	enum class ParseState
	{
		Normal,
		LeftBrace,
		RightBrace
	} state = ParseState::Normal;

	std::vector<FormatReplaceExpress> replaceExpresses;


	std::size_t leftBraceBegin = 0;

	std::size_t rightBraceBegin = 0;

	// 如果在表达式中出现左大括号
	std::size_t exprLeftCount = 0;


	for (std::size_t index = 0; index != fmt.size(); index++)
	{
		char ch = fmt[index];

		switch (state)
		{
		case ParseState::Normal:
			{
				if (ch == '{')
				{
					state = ParseState::LeftBrace;
					leftBraceBegin = index;
					exprLeftCount = 0;
				}
				else if (ch == '}')
				{
					state = ParseState::RightBrace;
					rightBraceBegin = index;
				}
				break;
			}
		case ParseState::LeftBrace:
			{
				if (ch == '{')
				{
					// 认为是左双大括号转义为可见的'{'
					if (index == leftBraceBegin + 1)
					{
						replaceExpresses.emplace_back("{", leftBraceBegin, index, false);
						state = ParseState::Normal;
					}
					else
					{
						exprLeftCount++;
					}
				}
				else if (ch == '}')
				{
					// 认为是表达式内的大括号
					if (exprLeftCount > 0)
					{
						exprLeftCount--;
						continue;
					}

					replaceExpresses.emplace_back(fmt.substr(leftBraceBegin + 1, index - leftBraceBegin - 1),
					                              leftBraceBegin, index, true);


					state = ParseState::Normal;
				}
				break;
			}
		case ParseState::RightBrace:
			{
				if (ch == '}' && (index == rightBraceBegin + 1))
				{
					replaceExpresses.emplace_back("}", rightBraceBegin, index, false);
				}
				else
				{
					//认为左右大括号失配，之前的不做处理，退格一位回去重新判断
					index--;
				}
				state = ParseState::Normal;
				break;
			}
		}
	}

	std::stringstream message;

	if (replaceExpresses.empty())
	{
		message << fmt;
	}
	else
	{
		// 拼接字符串
		std::size_t replaceCount = 0;
		std::size_t start = 0;
		for (std::size_t index = 0; index != replaceExpresses.size(); index++)
		{
			auto& replaceExpress = replaceExpresses[index];
			if (start < replaceExpress.StartIndex)
			{
				auto fragment = fmt.substr(start, replaceExpress.StartIndex - start);
				message << fragment;
				start = replaceExpress.StartIndex;
			}

			if (replaceExpress.NeedReplace)
			{
				if (replaceCount < argVec.size())
				{
					message << argVec[replaceCount++];
				}
			}
			else
			{
				message << replaceExpress.Expr;
			}

			start = replaceExpress.EndIndex + 1;
		}

		if (start < fmt.size())
		{
			auto fragment = fmt.substr(start, fmt.size() - start);
			message << fragment;
		}
	}
	return message.str();
}
}
