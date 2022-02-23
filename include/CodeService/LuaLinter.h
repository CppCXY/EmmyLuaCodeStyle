#include "LuaLinterOptions.h"

class LuaLinter
{
	LuaLinter(std::shared_ptr<LuaParser> luaParser, LuaLinterOptions& options);

	~LuaLinter() = default;

	void BuildLintElement();

	std::vector<LuaDiagnosisInfo> GetLintResult();
private:
	std::shared_ptr<LuaParser> _luaParser;
	LuaLinterOptions& _options;
};
