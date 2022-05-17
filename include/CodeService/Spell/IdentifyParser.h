#pragma once

#include <string>
#include <string_view>
#include <vector>

class IdentifyParser
{
public:
	enum class IdentifyType
	{
		Unknown,
		Ascii,
		Unicode,
		Ignore,
		LowerEnd,
		End,
	};

	struct WordRange
	{
		std::size_t Start;
		std::size_t Count;

		WordRange(std::size_t start, std::size_t count)
			: Start(start),
			  Count(count)
		{
		}
	};

	struct Word
	{
		WordRange Range;
		std::string Item;

		Word(const WordRange& range, const std::string& item)
			: Range(range),
			  Item(item)
		{
		}
	};

	IdentifyParser(std::string_view source);

	void Parse();

	std::vector<Word>& GetWords();
private:
	IdentifyType Lex();

	int GetCurrentChar();

	void PushWords(WordRange range);

	std::string_view _source;
	std::size_t _currentIndex;
	std::vector<Word> _words;
};
