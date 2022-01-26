#include "Util/Url.h"
#include "uriparser/Uri.h"

std::string url::UrlToFilePath(std::string_view url)
{
	std::string absUri(url);
	const int byteNeeded = static_cast<int>(url.size()) + 1;
	std::string filepath;
	filepath.resize(byteNeeded);
#ifdef _WIN32
	if (uriUriStringToWindowsFilenameA(absUri.c_str(), filepath.data()) != URI_SUCCESS)
	{
		return "";
	}
#else
	if (uriUriStringToUnixFilenameA(absUri.c_str(), filepath.data()) != URI_SUCCESS)
	{
		return "";
	}
#endif
	std::size_t validSize = strnlen(filepath.data(), filepath.size());
	filepath.resize(validSize);
	return filepath;
}

std::string url::FilePathToUrl(std::string_view path)
{
	std::string absFilename(path);
	const int byteNeeded = 8 + 3 * static_cast<int>(path.size()) + 1;
	std::string uri;
	uri.resize(byteNeeded);
#ifdef _WIN32
	if (uriWindowsFilenameToUriStringA(absFilename.c_str(), uri.data()) != URI_SUCCESS)
	{
		return "";
	}
#else
	if (uriUnixFilenameToUriStringA(absFilename.c_str(), uri.data()) != URI_SUCCESS)
	{
		return "";
	}
#endif
	std::size_t validSize = strnlen(uri.data(), uri.size());
	uri.resize(validSize);
	return uri;
}

std::string url::NormalizeUrl(std::string_view url)
{
	std::string oldUrl(url);
	UriUriA uri;
	const char* errorPos;
	if (uriParseSingleUriA(&uri, oldUrl.c_str(), &errorPos) != URI_SUCCESS)
	{
		// no need to call uriFreeUriMembersA
		return oldUrl;
	}

	const unsigned int normMask = URI_NORMALIZE_SCHEME | URI_NORMALIZE_USER_INFO;
	if (uriNormalizeSyntaxExA(&uri, normMask) != URI_SUCCESS)
	{
		uriFreeUriMembersA(&uri);
		return oldUrl;
	}

	int charsRequired = 0;

	if (uriToStringCharsRequiredA(&uri, &charsRequired) != URI_SUCCESS)
	{
		uriFreeUriMembersA(&uri);
		return oldUrl;
	}

	std::string newUrl;
	newUrl.resize(charsRequired * sizeof(char));

	int charsWrite = 0;
	if (uriToStringA(newUrl.data(), &uri, charsRequired, &charsWrite) != URI_SUCCESS)
	{
		uriFreeUriMembersA(&uri);
		return oldUrl;
	}

	uriFreeUriMembersA(&uri);
	newUrl.resize(charsWrite);
	return newUrl;
}
