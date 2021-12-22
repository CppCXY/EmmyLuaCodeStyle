#pragma once

#include <string>
#include <string_view>

namespace url {
	std::string UrlToFilePath(const std::string& url);

	std::string FilePathToUrl(const std::string& path);
}
