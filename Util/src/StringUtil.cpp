#include "Util/StringUtil.h"

std::vector<std::string_view> StringUtil::Split(std::string_view source, std::string_view separator)
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

std::string_view StringUtil::TrimSpace(std::string_view source)
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

	int end = source.size() - 1;
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

std::string StringUtil::Replace(std::string_view source, std::string_view oldString, std::string_view newString)
{
	if(oldString.empty())
	{
		return std::string(source);
	}

	std::string result;
	// 假设只有一次替换
	result.reserve(source.size() - oldString.size() + newString.size());

	while(true)
	{
		auto findPosition = source.find_first_of(oldString);
		result.append(source.substr(0, findPosition));
		if(findPosition == std::string_view::npos)
		{
			break;
		}
		result.append(newString);
		source = source.substr(findPosition + oldString.size());
	}

	return result;
}
