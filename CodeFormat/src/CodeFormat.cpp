#include <iostream>
#include <cstring>
#include <fstream>
#include "LuaParser/LuaParser.h"
#include "Util/format.h"
#include "CodeService/LuaFormatter.h"
#include "Util/CommandLine.h"

int main(int argc, char** argv)
{
	CommandLine cmd;

	cmd.AddTarget("format");
	cmd.AddTarget("diagnosis");

	cmd.Add<std::string>("file", "f", "Specify the input file");
	cmd.Add<int>("stdin", "i", "Read from stdin and specify read size");
	cmd.Add<std::string>("config", "c",
	                     "Specify editorconfig file, it decides on the effect of formatting or diagnosis");

	if (!cmd.Parse(argc, argv))
	{
		cmd.PrintUsage();
		return -1;
	}

	std::shared_ptr<LuaParser> parser = nullptr;

	if (!cmd.Get<std::string>("file").empty())
	{
		parser = LuaParser::LoadFromFile(argv[2]);
	}
	else if (cmd.Get<int>("stdin") != 0)
	{
		std::size_t size = cmd.Get<int>("stdin");

		std::string buffer;
		buffer.resize(size);
		std::cin.get(buffer.data(), size, EOF);
		auto realSize = strnlen(buffer.data(), size);
		buffer.resize(realSize);
		parser = LuaParser::LoadFromBuffer(std::move(buffer));
	}

	LuaCodeStyleOptions options;
	if (!cmd.Get<std::string>("config").empty())
	{
		options = LuaCodeStyleOptions::ParseFromEditorConfig(cmd.Get<std::string>("config"));
	}

	parser->BuildAstWithComment();

	LuaFormatter formatter(parser, options);
	formatter.BuildFormattedElement();

	if (cmd.GetTarget() == "format")
	{
		auto formattedText = formatter.GetFormattedText();
		std::cout.write(formattedText.data(), formattedText.size());
	}
	else if (cmd.GetTarget() == "diagnosis")
	{
		auto diagnosis = formatter.GetDiagnosisInfos();
		for (auto& d : diagnosis)
		{
			std::cout << format("{} from {}:{} to {}:{}", d.Message, d.Range.Start.Line, d.Range.Start.Character,
			                    d.Range.End.Line, d.Range.End.Character) << std::endl;
		}
	}
	return 0;
}
