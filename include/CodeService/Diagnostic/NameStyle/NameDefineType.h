#pragma once

enum class NameDefineType
{
	LocalVariableName,
    GlobalVariableDefineName,
    ClassVariableName,
	LocalFunctionName,
	ParamName,
	FunctionDefineName,
	ImportModuleName,
	ModuleDefineName,
	TableFieldDefineName,
};

struct NameStyleInfo {
    NameStyleInfo(NameDefineType type, std::size_t index)
            : Type(type), Index(index) {}

    NameDefineType Type;
    std::size_t Index;
};