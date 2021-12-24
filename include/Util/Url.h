#pragma once

#include <string>
#include <string_view>

namespace url {
	std::string UrlToFilePath(std::string_view url);

	std::string FilePathToUrl(std::string_view path);

	std::string NormalizeUrl(std::string_view url);
}
