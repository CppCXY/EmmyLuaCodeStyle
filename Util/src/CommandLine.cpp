#include "Util/CommandLine.h"
#include <iostream>
#include "Util/format.h"

CommandLineTargetOptions::CommandLineTargetOptions()
	: _enableRestArgs(false)
{
}

CommandLineTargetOptions& CommandLineTargetOptions::EnableKeyValueArgs()
{
	_enableRestArgs = true;
	return *this;
}

bool CommandLineTargetOptions::HasOption(std::string_view name)
{
	auto it = _args.find(name);
	if (it == _args.end())
	{
		return false;
	}

	return it->second.HasOption;
}

CommandLine::CommandLine()
	: _options(nullptr)
{
}

bool CommandLine::HasOption(std::string_view name)
{
	if (!_options)
	{
		return false;
	}

	return _options->HasOption(name);
}

CommandLineTargetOptions& CommandLine::AddTarget(std::string_view name)
{
	auto it = _targets.insert({std::string(name), CommandLineTargetOptions()});
	return it.first->second;
}

std::string_view CommandLine::GetTarget() const noexcept
{
	return _currentTarget;
}

std::string_view CommandLine::GetArg(int index) const noexcept
{
	if (static_cast<std::size_t>(index) < _argvs.size())
	{
		return _argvs[index];
	}
	else
	{
		return "";
	}
}

std::map<std::string, std::string, std::less<>>& CommandLine::GetKeyValueOptions()
{
	return _options->_restArgs;
}

bool CommandLine::Parse(int argc, char** argv)
{
	if (argc < 2)
	{
		return false;
	}
	_currentTarget = argv[1];

	if (_targets.count(_currentTarget) == 0)
	{
		return false;
	}

	_options = &_targets[_currentTarget];

	_argvs.reserve(argc);
	for (int i = 0; i != argc; i++)
	{
		_argvs.emplace_back(argv[i]);
	}

	// index = 0 �Ĳ����ǳ�����
	// index = 1 �Ĳ���ʱtarget
	for (int index = 2; index < argc; index++)
	{
		std::string_view current = _argvs[index];
		if (current.empty())
		{
			continue;
		}

		std::string_view option;

		if (current.starts_with("--"))
		{
			option = current.substr(2);
			if (option == "help")
			{
				return false;
			}

			auto eqPos = option.find_first_of('=');
			if (eqPos != std::string_view::npos)
			{
				if (!_options->_enableRestArgs)
				{
					_errors.emplace_back(util::format("Unknown option {} ,please enable key=value options", current));
					return false;
				}
				std::string_view value = option.substr(eqPos + 1);
				option = option.substr(0, eqPos);
				_options->_restArgs.insert({std::string(option), std::string(value)});
				continue;
			}
		}
		else if (current.starts_with("-"))
		{
			auto shortOption = current.substr(1);

			if (shortOption == "h")
			{
				return false;
			}

			if (_options->_shortMap.count(shortOption) == 0)
			{
				_errors.emplace_back(util::format("Unknown Option {}", current));
				return false;
			}
			option = _options->_shortMap.find(shortOption)->second;
		}
		else
		{
			return false;
		}

		if (index < argc - 1)
		{
			auto& commandOption = _options->_args.find(option)->second;
			if(_argvs[index + 1].starts_with("-"))
			{
				if(commandOption.Type == CommandLineValueType::Boolean)
				{
					commandOption.Value = "true";
					commandOption.HasOption = true;
					continue;
				}
				_errors.emplace_back(util::format("Option {} has not value", current));
				return false;
			}

			commandOption.Value = _argvs[++index];
			commandOption.HasOption = true;
		}
		else if(index == argc - 1)
		{
			auto& commandOption = _options->_args.find(option)->second;
			if (commandOption.Type == CommandLineValueType::Boolean)
			{
				commandOption.Value = "true";
				commandOption.HasOption = true;
				continue;
			}
			return false;
		}

	}
	return true;
}

void CommandLine::SetUsage(std::string_view usage)
{
	_usage = std::string(usage);
}

void CommandLine::PrintUsage()
{
	for (auto& error : _errors)
	{
		std::cerr << error << std::endl;
	}
	std::cerr << std::endl;

	std::cerr << _usage << std::endl;
	for (auto& target : _targets)
	{
		std::cerr << util::format("{}:", target.first) << std::endl;
		auto& options = target.second;
		for (auto& it : options._shortMap)
		{
			auto& shortName = it.first;
			auto& name = it.second;
			auto& option = options._args[name];

			std::cerr << util::format("\t-{} --{}\n\t\t{}",
				shortName, name, option.Description) << std::endl;
			std::cerr << std::endl;
		}
	}
}
