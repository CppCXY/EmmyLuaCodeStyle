#include <iostream>
#include <cstring>
#include <fstream>

#include "LuaFormat.h"
#include "Util/format.h"
#include "Util/CommandLine.h"
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

int main(int argc, char **argv) {
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
            .Add<bool>("overwrite", "ow", "Format overwrite the input file")
            .Add<std::string>("workspace", "w",
                              "Specify workspace directory,if no input file is specified, bulk formatting is performed")
            .Add<bool>("stdin", "i", "Read from stdin")
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
            .Add<bool>("name-style", "ns", "Enable name-style check")
            .EnableKeyValueArgs();


    if (!cmd.Parse(argc, argv)) {
        cmd.PrintUsage();
        return -1;
    }

    LuaFormat format;
    if (cmd.HasOption("file") || cmd.HasOption("stdin")) {
        format.SetWorkMode(WorkMode::File);
        if (cmd.HasOption("file") && !cmd.HasOption("stdin")) {
            if (!format.SetInputFile(cmd.Get<std::string>("file"))) {
                std::cerr << util::format("Can not find file {}", cmd.Get<std::string>("file")) << std::endl;
                return -1;
            }
        } else if (cmd.HasOption("stdin") && !cmd.HasOption("file")) {
            SET_BINARY_MODE();
            if (!format.ReadFromStdin()) {
                return -1;
            }
        } else {
            std::cerr << "Either --file or --stdin must be specified." << std::endl;
            return -1;
        }

        if (cmd.HasOption("outfile")) {
            format.SetOutputFile(cmd.Get<std::string>("outfile"));
        } else if (cmd.HasOption("overwrite")) {
            format.SetOutputFile(cmd.Get<std::string>("file"));
        }
    } else {
        format.SetWorkMode(WorkMode::Workspace);
    }

    if (cmd.HasOption("workspace")) {
        format.SetWorkspace(cmd.Get<std::string>("workspace"));
    }

    if (cmd.HasOption("ignores")) {
        auto ignores = cmd.Get<std::string>("ignores");
        auto patterns = string_util::Split(ignores, ";");
        for (auto pattern: patterns) {
            auto patternNoSpace = string_util::TrimSpace(pattern);
            if (!patternNoSpace.empty()) {
                format.AddIgnores(patternNoSpace);
            }
        }
    }

    if (cmd.HasOption("ignores-file")) {
        format.AddIgnoresByFile(cmd.Get<std::string>("ignores-file"));
    }

    if (cmd.Get<bool>("detect-config")) {
        format.AutoDetectConfig();
    } else if (cmd.HasOption("config")) {
        format.SetConfigPath(cmd.Get<std::string>("config"));
    } else {
        format.SetDefaultStyle(cmd.GetKeyValueOptions());
    }

    if(cmd.Get<bool>("name-style")){
        format.SupportNameStyleCheck();
    }

    if (cmd.GetTarget() == "format") {
        if (!format.Reformat()) {
            std::cerr << util::format("Exist lua syntax error") << std::endl;
            return -1;
        }
    } else if (cmd.GetTarget() == "check") {
        if (!format.Check() && cmd.Get<bool>("diagnosis-as-error")) {
            return -1;
        }
    }

    return 0;
}
