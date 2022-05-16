#pragma once
#include <string>

// Spelling suggestion returned from Lookup
class SuggestItem
{
public:
	static bool Comapare(const SuggestItem& lhs, const SuggestItem& rhs);

	SuggestItem();

	SuggestItem(std::string term, int distance, int count);

	// The suggested correctly spelled word.
	std::string Term;
	// Edit distance between searched for word and suggestion.
	int Distance = 0;
	// Frequency of suggestion in the dictionary (a measure of how common the word is).
	int Count = 0;
};
