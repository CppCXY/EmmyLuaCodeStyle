#pragma once
#include <string>

// Spelling suggestion returned from Lookup
class SuggestItem
{
public:
	SuggestItem()
	{
	}

	SuggestItem(std::string term, int distance, int64_t count)
		: Term(term),
		  Distance(distance),
		  Count(count)
	{
	}

	// The suggested correctly spelled word.</summary>
	std::string Term;
	// Edit distance between searched for word and suggestion.</summary>
	int Distance = 0;
	// Frequency of suggestion in the dictionary (a measure of how common the word is).</summary>
	int64_t Count = 0;
};
