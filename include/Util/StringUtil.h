#pragma once

#include <vector>
#include <string_view>
#include <string>

namespace StringUtil
{
	std::vector<std::string_view> Split(std::string_view source, std::string_view separator);

	std::string_view TrimSpace(std::string_view source);
}
