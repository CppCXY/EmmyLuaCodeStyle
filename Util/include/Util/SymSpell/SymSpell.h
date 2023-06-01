#pragma once

// Copyright (C) 2019 Wolf Garbe
// Version: 6.5
// Author: Wolf Garbe wolf.garbe@faroo.com
// Maintainer: Wolf Garbe wolf.garbe@faroo.com
// URL: https://github.com/wolfgarbe/symspell
// Description: https://medium.com/@wolfgarbe/1000x-faster-spelling-correction-algorithm-2012-8701fcd87a5f
//
// MIT License
// Copyright (c) 2019 Wolf Garbe
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// https://opensource.org/licenses/MIT

// algorithm come from https://github.com/AtheS21/SymspellCPP
// MODIFY: I re-implemented the whole algorithm
// @CppCXY: 我修改了整个算法的实现，精简了不需要的部分，修改了不够简练的部分

#include <map>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_set>
#include <unordered_map>
#include "SuggestItem.h"
#include "EditDistance.h"

class SymSpell
{
public:
	enum class Strategy
	{
		Normal,
		LazyLoaded
	};


	SymSpell(Strategy strategy,
	         int maxDictionaryEditDistance = 2,
	         int prefixLength = 3);

	~SymSpell();

	bool LoadWordDictionary(std::string path);

	bool LoadWordDictionaryFromBuffer(std::string_view buffer);

	bool LoadBuildInDictionary();

	bool CreateDictionaryEntry(const std::string& key, int count);

	bool IsCorrectWord(const std::string& word) const;

	std::vector<SuggestItem> LookUp(const std::string& input);

	std::vector<SuggestItem> LookUp(const std::string& input, int maxEditDistance);
private:
	bool BuildDeletesWords(const std::string& key);

	bool BuildAllDeletesWords();

	std::unordered_set<std::string> EditsPrefix(std::string_view key);

	void Edits(std::string_view word, int editDistance, std::unordered_set<std::string>& deleteWord);

	int GetStringHash(std::string_view source);

	bool DeleteInSuggestionPrefix(std::string_view deleteSuggest, std::size_t deleteLen, std::string_view suggestion,
	                              std::size_t suggestionLen);

	std::size_t _prefixLength; //prefix length  5..7
	uint32_t _compactMask;
	// DistanceAlgorithm distanceAlgorithm = DistanceAlgorithm::DamerauOSADistance;
	std::size_t _maxDictionaryWordLength; //maximum dictionary term length

	int _maxDictionaryEditDistance;
	// Dictionary that contains a mapping of lists of suggested correction words to the hashCodes
	// of the original words and the deletes derived from them. Collisions of hashCodes is tolerated,
	// because suggestions are ultimately verified via an edit distance function.
	// A list of suggestions might have a single suggestion, or multiple suggestions. 
	std::map<int, std::vector<std::string>> _deletes;
	// Dictionary of unique correct spelling words, and the frequency count for each word.
	std::unordered_map<std::string, int> _words;

	EditDistance _editDistance;

	Strategy _strategy;
};
