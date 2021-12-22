#include "Util/Url.h"
#include "uriparser/Uri.h"

std::string url::UrlToFilePath(const std::string& url)
{
	const char* absUri = url.c_str();
	const int byteNeeded = url.size() + 1;
	std::string filepath;
	filepath.resize(byteNeeded);
#ifdef _WIN32
	if (uriUriStringToWindowsFilenameA(absUri, filepath.data()) != URI_SUCCESS)
	{
		return "";
	}
#else
	if (uriUriStringToUnixFilenameA(absUri, filepath.data()) != URI_SUCCESS)
	{
		return "";
	}
#endif
	std::size_t validSize = strnlen(filepath.data(), filepath.size());
	filepath.resize(validSize);
	return filepath;
}

std::string url::FilePathToUrl(const std::string& path)
{
	const char* absFilename = path.c_str();
	const int byteNeeded = 8 + 3 * path.size() + 1;
	std::string uri;
	uri.resize(byteNeeded);
#ifdef _WIN32
	if(uriWindowsFilenameToUriStringA(absFilename, uri.data()) != URI_SUCCESS)
	{
		return "";
	}
#else
	if (uriUnixFilenameToUriStringA(absFilename, uri.data()) != URI_SUCCESS)
	{
		return "";
	}
#endif
	std::size_t validSize = strnlen(uri.data(), uri.size());
	uri.resize(validSize);
	return uri;
}
