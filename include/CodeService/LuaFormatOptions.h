#pragma once

class LuaFormatOptions
{
public:
	// 我反对这种风格
	// 但我依然实现他
	bool UseTabIndent = false;

	int Indent = 4;
	// 这只是初始化时的默认选项，在linux上依然可以保持 \r\n
#ifdef _WINDOWS
	std::string LineSeperater = "\r\n";
#else
	std::string LineSeperater = "\r";
#endif
};
