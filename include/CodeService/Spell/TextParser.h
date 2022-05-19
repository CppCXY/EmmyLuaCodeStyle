#pragma once
#include "BasicSpellParserType.h"
#include <string_view>
#include <vector>

namespace spell {
// TODO Write later
class TextParser
{
public:
	enum class TextType
	{
		Unknown,
		End,
	};

	TextParser(std::string_view source);

	void Parse();

	std::vector<spell::Word>& GetWords();

private:
	TextType Lex();
};
}
