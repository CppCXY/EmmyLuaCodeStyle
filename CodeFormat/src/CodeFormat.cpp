#include <iostream>
#include <cstring>
#include <fstream>

#include "LuaFormat.h"
#include "CodeService/LuaEditorConfig.h"
#include "LuaParser/LuaParser.h"
#include "Util/format.h"
#include "CodeService/LuaFormatter.h"
#include "Util/CommandLine.h"
#include "LuaWorkspaceFormat.h"
#include "Util/StringUtil.h"

// https://stackoverflow.com/questions/1598985/c-read-binary-stdin
#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
# define SET_BINARY_MODE() _setmode(_fileno(stdin), _O_BINARY);\
	_setmode(_fileno(stdout), _O_BINARY)
#else
# define SET_BINARY_MODE() ((void)0)
#endif


int main(int argc, char** argv)
{
	CommandLine cmd;
	cmd.SetUsage(
		"Usage:\n"
		"CodeFormat [check/format] [options]\n"
		"for example:\n"
		"\tCodeFormat check -w . -d\n"
		"\tCodeFormat format -f test.lua -d\n"
		"\tCodeFormat check -w . -d --ignores \"Test/*.lua;src/**.lua\"\n"
		"\tCodeFormat check -w . -d --ignores-file \".gitignore\"\n"
	);
	cmd.AddTarget("format")
	   .Add<std::string>("file", "f", "Specify the input file")
	   .Add<std::string>("overwrite", "ow", "Format overwrite the input file")
	   .Add<std::string>("workspace", "w",
	                     "Specify workspace directory,if no input file is specified, bulk formatting is performed")
	   .Add<int>("stdin", "i", "Read from stdin and specify read size")
	   .Add<std::string>("config", "c",
	                     "Specify .editorconfig file, it decides on the effect of formatting")
	   .Add<bool>("detect-config", "d",
	              "Configuration will be automatically detected,\n"
	              "\t\tIf this option is set, the config option has no effect ")
	   .Add<std::string>("outfile", "o",
	                     "Specify output file")
	   .Add<std::string>("ignores-file", "igf",
	                     "Specify which files to ignore through configuration file,for example \".gitignore\""
	   )
	   .Add<std::string>("ignores", "ig",
	                     "Use file wildcards to specify how to ignore files\n"
	                     "\t\tseparated by ';'"
	   )
	   .EnableKeyValueArgs();
	cmd.AddTarget("check")
	   .Add<std::string>("file", "f", "Specify the input file")
	   .Add<std::string>("workspace", "w",
	                     "Specify workspace directory, if no input file is specified, bulk checking is performed")
	   .Add<std::string>("config", "c",
	                     "Specify editorconfig file, it decides on the effect of formatting or diagnosis")
	   .Add<bool>("detect-config", "d",
	              "Configuration will be automatically detected,\n"
	              "\t\tIf this option is set, the config option has no effect")
	   .Add<bool>("diagnosis-as-error", "DAE", "if exist error or diagnosis info , return -1")
	   .Add<std::string>("ignores-file", "igf",
	                     "Specify which files to ignore through configuration file,for example \".gitignore\""
	   )
	   .Add<std::string>("ignores", "ig",
	                     "Use file wildcards to specify how to ignore files\n"
	                     "\t\tseparated by ';'"
	   )
	   // .Add<std::string>("output", "o",
	   //                   "Diagnostic messages are output to file or standard io"
	   //                   "\t\toptional filename/stdout/stderr, default is stderr")
	   .EnableKeyValueArgs();


	if (!cmd.Parse(argc, argv))
	{
		cmd.PrintUsage();
		return -1;
	}

	if (cmd.HasOption("file"))
	{
		auto luaFormat = std::make_shared<LuaFormat>();
		if (cmd.HasOption("file"))
		{
			if (!luaFormat->SetInputFile(cmd.Get<std::string>("file")))
			{
				std::cerr << Util::format("Can not find file {}", cmd.Get<std::string>("file")) << std::endl;
				return -1;
			}
		}
		else if (cmd.HasOption("stdin"))
		{
			SET_BINARY_MODE();
			std::size_t size = cmd.Get<int>("stdin");
			if (!luaFormat->ReadFromStdin(size))
			{
				return -1;
			}
		}
		else
		{
			std::cerr << "not special input file" << std::endl;
			return -1;
		}

		if (cmd.HasOption("outfile"))
		{
			luaFormat->SetOutputFile(cmd.Get<std::string>("outfile"));
		}
		else if (cmd.HasOption("overwrite"))
		{
			luaFormat->SetOutputFile(cmd.Get<std::string>("file"));
		}

		if (cmd.Get<bool>("detect-config"))
		{
			if (cmd.HasOption("workspace"))
			{
				luaFormat->AutoDetectConfig(std::filesystem::path(cmd.Get<std::string>("workspace")));
			}
			else
			{
				luaFormat->AutoDetectConfig();
			}
		}
		else if (cmd.HasOption("config"))
		{
			luaFormat->SetConfigPath(cmd.Get<std::string>("config"));
		}

		luaFormat->SetDefaultOptions(cmd.GetKeyValueOptions());

		if (cmd.GetTarget() == "format")
		{
			if (!luaFormat->Reformat())
			{
				std::cerr << Util::format("Exist lua syntax error") << std::endl;
				return -1;
			}
		}
		else if (cmd.GetTarget() == "check")
		{
			if (!luaFormat->Check() && cmd.Get<bool>("diagnosis-as-error"))
			{
				return -1;
			}
		}
	}
	else if (cmd.HasOption("workspace"))
	{
		LuaWorkspaceFormat workspaceFormat(cmd.Get<std::string>("workspace"));

		if (cmd.Get<bool>("detect-config"))
		{
			workspaceFormat.SetAutoDetectConfig(true);
		}
		else if (cmd.HasOption("config"))
		{
			workspaceFormat.SetConfigPath(cmd.Get<std::string>("config"));
		}

		if (cmd.HasOption("ignores"))
		{
			auto ignores = cmd.Get<std::string>("ignores");
			auto patterns = StringUtil::Split(ignores, ";");
			for (auto pattern : patterns)
			{
				auto patternNoSpace = StringUtil::TrimSpace(pattern);
				if (!patternNoSpace.empty())
				{
					workspaceFormat.AddIgnores(patternNoSpace);
				}
			}
		}

		if (cmd.HasOption("ignores-file"))
		{
			workspaceFormat.AddIgnoresByFile(cmd.Get<std::string>("ignores-file"));
		}

		workspaceFormat.SetKeyValues(cmd.GetKeyValueOptions());

		if (cmd.GetTarget() == "format")
		{
			workspaceFormat.ReformatWorkspace();
		}
		else if (cmd.GetTarget() == "check")
		{
			if (!workspaceFormat.CheckWorkspace() && cmd.Get<bool>("diagnosis-as-error"))
			{
				return -1;
			}
		}
	}
	return 0;
}
