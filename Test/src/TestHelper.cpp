#include "TestHelper.h"

std::string TestHelper::ScriptBase = "";

LuaStyle TestHelper::DefaultStyle;

bool TestHelper::TestFormatted(std::string input, const std::string& shouldBe, LuaStyle& style)
{
    auto file = std::make_shared<LuaFile>(std::move(input));
    LuaLexer luaLexer(file);
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();

    LuaSyntaxTree t;
    t.BuildTree(p);

    LuaStyle s;
    FormatBuilder b(s);
    auto text = b.GetFormatResult(t);

	return string_util::TrimSpace(text) == string_util::TrimSpace(shouldBe);
}
//
//uint64_t TestPerformance(std::string input, std::shared_ptr<LuaCodeStyleOptions> options)
//{
//	auto start = steady_clock::now();
//	auto parser = LuaParser::LoadFromBuffer(std::move(input));
//	parser->BuildAstWithComment();
//
//	LuaFormatter formatter(parser, *options);
//
//	formatter.BuildFormattedElement();
//
//	volatile auto formattedText = formatter.GetFormattedText();
//
//	return duration_cast<milliseconds>(steady_clock::now() - start).count();
//}
//
//bool TestGrammar(std::string input)
//{
//	auto parser = LuaParser::LoadFromBuffer(std::move(input));
//	parser->BuildAstWithComment();
//	bool result = parser->GetErrors().empty();
//
//	if (!result)
//	{
//		for (auto& error : parser->GetErrors())
//			std::cout << Util::format("error: {}, from [{},{}] to [{},{}]", error.ErrorMessage,
//                                      parser->GetStartLine(error.ErrorRange.StartOffset),
//			                    parser->GetColumn(error.ErrorRange.StartOffset),
//                                      parser->GetStartLine(error.ErrorRange.EndOffset),
//			                    parser->GetColumn(error.ErrorRange.EndOffset)
//			) << std::endl;
//		return result;
//	}
//
//	auto options = std::make_shared<LuaCodeStyleOptions>();
//
//	LuaFormatter formatter(parser, *options);
//
//	formatter.BuildFormattedElement();
//
//	auto formattedText = formatter.GetFormattedText();
//	auto formattedParser = LuaParser::LoadFromBuffer(std::move(formattedText));
//
//	formattedParser->BuildAstWithComment();
//	result = formattedParser->GetErrors().empty();
//
//	if (!result)
//	{
//		for (auto& error : formattedParser->GetErrors())
//			std::cout << Util::format("error after formatted: {}, from [{},{}] to [{},{}]", error.ErrorMessage,
//                                      formattedParser->GetStartLine(error.ErrorRange.StartOffset),
//			                    formattedParser->GetColumn(error.ErrorRange.StartOffset),
//                                      formattedParser->GetStartLine(error.ErrorRange.EndOffset),
//			                    formattedParser->GetColumn(error.ErrorRange.EndOffset)
//			) << std::endl;
//	}
//
//	return result;
//}
//
void TestHelper::CollectLuaFile(std::filesystem::path directoryPath, std::vector<std::string>& paths, std::filesystem::path& root)
{
	if (!std::filesystem::exists(directoryPath))
	{
		return;
	}

	for (auto& it : std::filesystem::directory_iterator(directoryPath))
	{
		if (std::filesystem::is_directory(it.status()))
		{
			CollectLuaFile(it.path().string(), paths, root);
		}
		else if (it.path().extension() == ".lua")
		{
			paths.push_back(std::filesystem::relative(it.path(), root).string());
		}
	}
}

std::string TestHelper::ReadFile(const std::string& path)
{
    std::string newPath = ScriptBase + "/" + path;
#ifdef _WIN32
	std::fstream fin(newPath, std::ios::in | std::ios::binary);
#else
	std::fstream fin(path, std::ios::in);
#endif

	if (fin.is_open())
	{
		std::stringstream s;
		s << fin.rdbuf();
		return s.str();
	}

	return "";
}

LuaParser TestHelper::GetParser(std::string input) {
    auto file = std::make_shared<LuaFile>(std::move(input));
    LuaLexer luaLexer(file);
    luaLexer.Parse();

    LuaParser p(file, std::move(luaLexer.GetTokens()));
    p.Parse();
    return std::move(p);
}
