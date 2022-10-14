#pragma once

#include <string>
#include <map>
#include <type_traits>
#include <vector>
#include <set>


class CommandLine;

enum class CommandLineValueType {
    Boolean = 0,
    Int = 1,
    String = 2,
};

struct CommandLineOption {
    std::string Value;
    std::string Description;
    CommandLineValueType Type;
    bool HasOption;
};

class CommandLineTargetOptions {
public:
    CommandLineTargetOptions();

    /**
     * @brief 添加解析的字符串参数
     */
    template<typename T>
    CommandLineTargetOptions &Add(const std::string_view name, std::string_view shortName,
                                  std::string_view description) {
        CommandLineValueType type = CommandLineValueType::Boolean;
        if constexpr (std::is_same_v<T, bool>) {
            type = CommandLineValueType::Boolean;
        } else if constexpr (std::is_same_v<T, int>) {
            type = CommandLineValueType::Int;
        } else if constexpr (std::is_same_v<T, std::string>) {
            type = CommandLineValueType::String;
        } else {
            return *this;
        }

        _shortMap.insert({std::string(shortName), std::string(name)});
        _args.insert({
                             std::string(name),
                             CommandLineOption{"", std::string(description), type, false}
                     });
        return *this;
    }

    CommandLineTargetOptions &EnableKeyValueArgs();

private:
    template<typename T>
    T Get(std::string_view name) {
        if (!HasOption(name)) {
            return T();
        }

        auto it = _args.find(name);
        auto &arg = it->second;
        if constexpr (std::is_same_v<T, bool>) {
            return arg.Value == "true";
        } else if constexpr (std::is_same_v<T, int>) {
            return std::stoi(arg.Value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg.Value;
        }

        return {};
    }

    bool HasOption(std::string_view name);

    friend CommandLine;
    std::map<std::string, CommandLineOption, std::less<>> _args;
    std::map<std::string, std::string, std::less<>> _shortMap;
    std::map<std::string, std::string, std::less<>> _restArgs;
    bool _enableRestArgs;
};

/**
 * @brief 特定语法结构的命令行解析工具
 */
class CommandLine {
public:
    CommandLine();

    template<typename T>
    T Get(std::string_view name) {
        return _options->Get<T>(name);
    }

    bool HasOption(std::string_view name);

    /**
     * @brief 添加解析目标
     */
    CommandLineTargetOptions &AddTarget(std::string_view name);

    std::string_view GetTarget() const noexcept;

    /**
     * @brief 按顺序获取参数
     */
    std::string_view GetArg(int index) const noexcept;

    std::map<std::string, std::string, std::less<>> &GetKeyValueOptions();

    bool Parse(int argc, char **argv);

    void SetUsage(std::string_view usage);

    void PrintUsage();

private:
    std::vector<std::string> _argvs;
    std::map<std::string, CommandLineTargetOptions, std::less<>> _targets;
    CommandLineTargetOptions *_options;
    std::string _currentTarget;
    std::vector<std::string> _errors;
    std::string _usage;
};

