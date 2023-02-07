#include "Util/StringUtil.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include "wildcards/match.hpp"

std::vector<std::string_view> string_util::Split(std::string_view source, std::string_view separator)
{
	if (source.empty() || separator.empty())
	{
		return {};
	}

	std::size_t sepLen = separator.size();
	std::vector<std::string_view> result;

	while (true)
	{
		const std::size_t sepIndex = source.find_first_of(separator, 0);
		if (sepIndex == std::string_view::npos)
		{
			result.push_back(source);
			break;
		}
		else
		{
			result.push_back(source.substr(0, sepIndex));
			source = source.substr(sepIndex + sepLen);
		}
	}
	return result;
}

std::string_view string_util::TrimSpace(std::string_view source)
{
	if (source.empty())
	{
		return source;
	}

	std::size_t start = 0;
	for (; start != source.size(); start++)
	{
		char ch = source[start];
		if (ch != '\n' && ch != '\r' && ch != ' ' && ch != '\t' && ch != '\v' && ch != '\f')
		{
			break;
		}
	}

	int end = static_cast<int>(source.size()) - 1;
	for (; end >= 0; end--)
	{
		char ch = source[end];
		if (ch != '\n' && ch != '\r' && ch != ' ' && ch != '\t' && ch != '\v' && ch != '\f')
		{
			break;
		}
	}

	if (end < static_cast<int>(start))
	{
		return "";
	}
	return source.substr(start, end - start + 1);
}

std::string string_util::Replace(std::string_view source, std::string_view oldString, std::string_view newString)
{
	if (oldString.empty())
	{
		return std::string(source);
	}

	std::string result;

	result.reserve(source.size() - oldString.size() + newString.size());

	while (true)
	{
		auto findPosition = source.find_first_of(oldString);
		result.append(source.substr(0, findPosition));
		if (findPosition == std::string_view::npos)
		{
			break;
		}
		result.append(newString);
		source = source.substr(findPosition + oldString.size());
	}

	return result;
}

bool string_util::StartWith(std::string_view source, std::string_view str)
{
	if (source.size() < str.size())
	{
		return false;
	}

	return strncmp(source.data(), str.data(), str.size()) == 0;
}

bool string_util::EndWith(std::string_view source, std::string_view str)
{
	if (source.size() < str.size())
	{
		return false;
	}

	return strncmp(source.data() + source.size() - str.size(), str.data(), str.size()) == 0;
}

bool string_util::IsStringEqualIgnoreCase(std::string_view lhs, std::string_view rhs)
{
	if (lhs.size() != rhs.size())
	{
		return false;
	}
	for (std::size_t i = 0; i != lhs.size(); i++)
	{
		if (::tolower(lhs[i]) != ::tolower(rhs[i]))
		{
			return false;
		}
	}
	return true;
}

std::string_view string_util::GetFileRelativePath(std::string_view workspace, std::string_view filePath)
{
	if (workspace.size() >= filePath.size())
	{
		return "";
	}

	std::size_t i = 0;
	std::size_t size = workspace.size();
	for (; i != size; i++)
	{
		char wch = workspace[i];
		char fch = filePath[i];
		if (wch != fch)
		{
			if ((wch == '\\' || wch == '/') && (fch == '\\' || fch == '/'))
			{
				continue;
			}
			return "";
		}
	}

	if (i < filePath.size() 
		&& (filePath[i] == '\\' || filePath[i] == '/'))
	{
		i++;
	}

	return std::string_view(filePath.data() + i, filePath.size() - i);
}

struct equal_to
{
	constexpr auto operator()(const char& lhs, const char& rhs) const -> decltype(lhs == rhs)
	{
		return lhs == rhs
			|| ((lhs == '\\' || lhs == '/') && (rhs == '\\' || rhs == '/'))
			|| (::tolower(lhs) == ::tolower(rhs));
	}
};

bool string_util::FileWildcardMatch(std::string_view sourceFile, std::string_view pattern)
{
	equal_to eq;

	auto minSize = std::min(sourceFile.size(), pattern.size());
	bool match = true;
	for (std::size_t i = 0; i != minSize; i++)
	{
		if(!eq(sourceFile[i], pattern[i]))
		{
			match = false;
			break;
		}
	}

	if(!match)
	{
		match = wildcards::match(sourceFile, pattern, eq).res;
	}
	return match;
}

// glibc
int __strncasecmp(const char* s1, const char* s2, int n)
{
	if (n && s1 != s2)
	{
		do
		{
			int d = ::tolower(*s1) - ::tolower(*s2);
			if (d || *s1 == '\0' || *s2 == '\0') return d;
			s1++;
			s2++;
		} while (--n);
	}
	return 0;
}

bool string_util::CaseInsensitiveLess::operator()(std::string_view lhs, std::string_view rhs) const
{
	std::size_t llen = lhs.size();
	std::size_t rlen = rhs.size();

	int ret = __strncasecmp(lhs.data(), rhs.data(), static_cast<int>((std::min)(llen, rlen)));

	if (ret < 0)
	{
		return true;
	}
	if (ret == 0 && llen < rlen)
	{
		return true;
	}
	return false;
}
