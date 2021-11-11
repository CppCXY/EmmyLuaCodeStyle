#pragma once

#include <string>
#include <map>
#include <type_traits>
#include <vector>
#include <set>

enum class CommandLineValueType
{
	Boolean = 0,
	Int = 1,
	String = 2,
};

struct CommandLineOption
{
	std::string Value;
	std::string Description;
	CommandLineValueType Type;
};

/**
 * @brief 特定语法结构的命令行解析工具
 */
class CommandLine
{
public:
	/**
	 * @brief 添加解析的字符串参数
	 */
	template <typename T>
	bool Add(const std::string_view name, std::string_view shortName,
	         std::string_view description)
	{
		CommandLineValueType type = CommandLineValueType::Boolean;
		if constexpr (std::is_same_v<T, bool>)
		{
			type = CommandLineValueType::Boolean;
		}
		else if constexpr (std::is_same_v<T, int>)
		{
			type = CommandLineValueType::Int;
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			type = CommandLineValueType::String;
		}
		else
		{
			return false;
		}

		_shortMap.insert({std::string(shortName),std::string(name)});
		_args.insert({ std::string(name), CommandLineOption("", std::string(description), type)});
		return true;
	}

	template <typename T>
	T Get(std::string_view name)
	{
		std::string key(name);
		if (_args.count(key) == 0)
		{
			return T();
		}

		const CommandLineOption& arg = _args[key];
		if constexpr (std::is_same_v<T, bool>)
		{
			return arg.Value == "true";
		}
		else if constexpr (std::is_same_v<T, int>)
		{
			return std::stoi(arg.Value);
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			return arg.Value;
		}

		return {};
	}

	/**
	 * @brief 添加解析目标
	 */
	void AddTarget(const std::string& name);

	std::string GetTarget() const noexcept;

	/**
	 * @brief 按顺序获取参数
	 */
	std::string GetArg(int index) const noexcept;

	bool Parse(int argc, char** argv);

	void PrintUsage();
private:
	std::map<std::string, CommandLineOption> _args;
	std::map<std::string, std::string> _shortMap;

	std::vector<std::string> _argvs;
	std::set<std::string> _targets;
	std::string _currentTarget;
	std::vector<std::string> _errors;
};
