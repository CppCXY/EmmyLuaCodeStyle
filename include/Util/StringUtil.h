#pragma once

#include <vector>
#include <string_view>
#include <string>

namespace StringUtil
{
	std::vector<std::string_view> Split(std::string_view source, std::string_view separator);

	std::string_view TrimSpace(std::string_view source);

	std::string Replace(std::string_view source, std::string_view oldString, std::string_view newString);

	// workaround for C++17
	bool StartWith(const std::string& source, const std::string& str);
	// workaround for C++17
	bool EndWith(const std::string& source, const std::string& str);

	bool IsStringEqualIgnoreCase(std::string_view lhs, std::string_view rhs);
}
