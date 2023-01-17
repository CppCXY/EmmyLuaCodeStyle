#pragma once

#include <cassert>
#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <chrono>
#include "Util/format.h"
#include "Util/CommandLine.h"
#include "Util/StringUtil.h"
#include "CodeService/Config/LuaEditorConfig.h"
#include "LuaParser/Parse/LuaParser.h"
#include "CodeService/Format/FormatBuilder.h"

class TestHelper {
public:
    static bool TestFormatted(std::string input, const std::string &shouldBe, LuaStyle &style);

    static std::string ReadFile(const std::string &path);

    static LuaParser GetParser(std::string input);

    static void
    CollectLuaFile(std::filesystem::path directoryPath, std::vector<std::string> &paths, std::filesystem::path &root);

    static std::string ScriptBase;
};