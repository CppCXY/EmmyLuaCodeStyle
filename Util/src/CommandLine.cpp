#include "Util/CommandLine.h"
#include <iostream>
#include "Util/format.h"

void CommandLine::AddTarget(const std::string& name)
{
	_targets.insert(name);
}

std::string CommandLine::GetTarget() const noexcept
{
	return _currentTarget;
}

std::string CommandLine::GetArg(int index) const noexcept
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

	_argvs.reserve(argc);
	for (int i = 0; i != argc; i++)
	{
		_argvs.emplace_back(argv[i]);
	}

	// index = 0 的参数是程序名
	// index = 1 的参数时target
	for (int index = 2; index < argc; index++)
	{
		std::string& current = _argvs[index];
		if (current.empty())
		{
			continue;
		}

		std::string option;

		// 其实应该用start_with
		if (current.starts_with("--"))
		{
			option = current.substr(2);
			if (option == "help")
			{
				return false;
			}
		}
		else if (current.starts_with("-"))
		{
			auto shortOption = current.substr(1);

			if (shortOption == "h")
			{
				return false;
			}

			if (!_shortMap.count(shortOption))
			{
				_errors.emplace_back(format("Unknown Option {}", current));
				return false;
			}

			option = _shortMap[shortOption];
		}
		else
		{
			return false;
		}

		if (index < argc - 1)
		{
			_args[option].Value = _argvs[++index];
		}
	}
	return true;
}

void CommandLine::PrintUsage()
{
	for (auto& error : _errors)
	{
		std::cerr << error << std::endl;
	}
	std::cerr << std::endl;

	std::cerr << "first param must be target:" << std::endl;
	for (auto& target : _targets)
	{
		std::cerr << format("{}", target) << std::endl;
	}

	std::cerr << std::endl;

	for (auto& it : _shortMap)
	{
		auto& shortName = it.first;
		auto& name = it.second;
		auto& option = _args[name];

		std::cerr << format("-{} --{} {}",
		                    shortName, name, option.Description) << std::endl;
	}
}
