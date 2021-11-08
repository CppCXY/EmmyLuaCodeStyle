#pragma once

class LuaFormatOptions
{
public:
	/*
	 * 我反对这种风格
	 * 但我依然实现他
	 */ 
	bool UseTabIndent = false;

	/*
	 * 缩进的空白数
	 */ 
	int Indent = 4;

	/*
	 * 调用参数对齐到第一个参数，经过实际体验这种风格在vscode上会因为垂直对齐线的标注而显得极为难看
	 * 
	 */
	bool AlignCallArgs = false;

	/*
	 * 函数调用的括号和参数之间保持一个空格
	 *
	 */
	bool BlankBetweenCallArgsAndBracket = false;

	/*
	 * 函数定义的参数保持对齐到第一个参数
	 * 函数的定义通常不会太长，这种默认行为是可以接受的
	 */
	bool AlignFunctionDefineParams = true;

	/*
	 * 表的大括号和表项之间保持一个空格
	 * 经过实际体验这种方式比较美观
	 */
	bool BlankBetweenTableFieldsAndBracket = true;

	/*
	 * 表内每一个表项对齐到第一个表项
	 */
	bool AlignTableFieldToFirst = true;

	/*
	 * 这可太蠢了
	 */
	// bool KeepSingleQuote = false;
	// bool KeepDoubleQuote = false;
	// bool KeepTableFieldSepComma = false;
	// bool KeepTableFieldSepSemicolon = false;

	/*
	 * 包含语句块的语句和下一个语句之间保持最小1行
	 */
	int BlockStatementKeepMinLine = 1;

	// 这只是初始化时的默认选项，在linux上依然可以保持 \r\n
#ifdef _WINDOWS
	std::string LineSeperater = "\r\n";
#else
	std::string LineSeperater = "\n";
#endif
};
