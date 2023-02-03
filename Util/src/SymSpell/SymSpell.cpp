#include "Util/SymSpell/SymSpell.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>
#include <cmath>
#include <algorithm>
#include "Util/StringUtil.h"

// extern const char *BuildInWords;

SymSpell::SymSpell(Strategy strategy,
                   int maxDictionaryEditDistance,
                   int prefixLength)
	: _prefixLength(prefixLength),
	  _compactMask((std::numeric_limits<uint32_t>::max() >> 8) << 2),
	  _maxDictionaryWordLength(0),
	  _maxDictionaryEditDistance(maxDictionaryEditDistance),
	  _words(),
	  _strategy(strategy)
{
}

SymSpell::~SymSpell()
{
}

bool SymSpell::LoadWordDictionary(std::string path)
{
	std::fstream fin(path, std::ios::in);
	if (!fin.is_open())
	{
		return false;
	}

	std::stringstream s;
	s << fin.rdbuf();

	return LoadWordDictionaryFromBuffer(s.str());
}

bool SymSpell::LoadWordDictionaryFromBuffer(std::string_view buffer)
{
	auto lines = string_util::Split(buffer, "\n");
	try
	{
		for (auto& line : lines)
		{
			auto tokens = string_util::Split(line, " ");
			if (tokens.empty())
			{
				continue;
			}
			if(tokens.size() == 2)
			{
				int64_t count = std::stoll(std::string(tokens[1]));
				if (count > std::numeric_limits<int>::max())
				{
					count = std::numeric_limits<int>::max();
				}
				CreateDictionaryEntry(std::string(tokens[0]), static_cast<int>(count));
			}
			else
			{
				CreateDictionaryEntry(std::string(tokens[0]), 1);
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}

	return true;
}

bool SymSpell::LoadBuildInDictionary()
{
	// std::string_view dictionarySource = BuildInWords;
	//
	// auto lines = string_util::Split(dictionarySource, "\n");
	// try
	// {
	// 	for (auto& line : lines)
	// 	{
	// 		auto tokens = string_util::Split(line, " ");
	// 		if (tokens.size() < 2)
	// 		{
	// 			continue;
	// 		}
	//
	// 		int64_t count = std::stoll(std::string(tokens[1]));
	// 		if (count > std::numeric_limits<int>::max())
	// 		{
	// 			count = std::numeric_limits<int>::max();
	// 		}
	// 		CreateDictionaryEntry(tokens[0], static_cast<int>(count));
	// 	}
	// }
	// catch (std::exception& e)
	// {
	// 	std::cerr << e.what() << std::endl;
	// 	return false;
	// }

	return true;
}

bool SymSpell::CreateDictionaryEntry(const std::string& key, int count)
{
	if (count <= 0)
	{
		return false;
	}

	// modify: �������ʽ������ж�
	auto wordsFounded = _words.find(key);
	if (wordsFounded != _words.end())
	{
		int countPrevious = wordsFounded->second;
		count = (std::numeric_limits<int>::max() - countPrevious > count)
			        ? (countPrevious + count)
			        : std::numeric_limits<int>::max();
		wordsFounded->second = count;
		return false;
	}
	else
	{
		_words.insert({key, count});
	}

	if(_strategy == Strategy::LazyLoaded)
	{
		return true;
	}

	return BuildDeletesWords(key);
}

bool SymSpell::IsCorrectWord(const std::string& word) const
{
	return (word.size() <= static_cast<std::size_t>(_maxDictionaryEditDistance)) || (_words.count(word) == 1);
}

std::vector<SuggestItem> SymSpell::LookUp(const std::string& input)
{
	return LookUp(input, _maxDictionaryEditDistance);
}

std::vector<SuggestItem> SymSpell::LookUp(const std::string& input, int maxEditDistance)
{
	if(_strategy == Strategy::LazyLoaded)
	{
		BuildAllDeletesWords();
		_strategy = Strategy::Normal;
	}

	std::vector<SuggestItem> suggestions;

	if (maxEditDistance > _maxDictionaryEditDistance)
	{
		return suggestions;
	}

	// early exit - word is too big to possibly match any words
	if (input.size() - 1 > _maxDictionaryWordLength)
	{
		return suggestions;
	}

	int suggestionCount = 0;
	auto founded = _words.find(input);

	if (founded != _words.end())
	{
		// suggestionCount = founded->second;
		// suggestions.emplace_back(std::string(input), 0, suggestionCount);
		// Modify: ������Ѿ����ڵĵ����򲻸�������
		return suggestions;
	}

	// deletes we've considered already
	std::unordered_set<std::string> hashset1;
	// suggestions we've considered already
	std::unordered_set<std::string> hashset2;

	hashset2.insert(std::string(input));

	int maxEditDistance2 = maxEditDistance;

	std::vector<std::string> candidates;

	auto inputPrefixLen = input.size();
	auto inputLen = inputPrefixLen;
	if (inputPrefixLen > _prefixLength)
	{
		inputPrefixLen = _prefixLength;
		candidates.emplace_back(input.substr(0, inputPrefixLen));
	}
	else
	{
		candidates.emplace_back(input);
	}

	std::size_t candidateIndex = 0;
	while (candidateIndex < candidates.size())
	{
		// do not &
		auto candidate = candidates[candidateIndex++];
		auto candidateLen = candidate.size();
		int lengthDiff = static_cast<int>(inputPrefixLen - candidateLen);
		if (lengthDiff > maxEditDistance2)
		{
			break;
		}

		auto it = _deletes.find(GetStringHash(candidate));
		if (it != _deletes.end())
		{
			auto& dictSuggestions = it->second;
			for (auto& suggestion : dictSuggestions)
			{
				auto suggestionLen = suggestion.size();
				if (suggestion == input)
				{
					continue;
				}

				if ((::abs(static_cast<int>(suggestionLen - input.size())) > maxEditDistance2)
					// input and sugg lengths diff > allowed/current best distance
					|| (suggestionLen < candidateLen)
					// sugg must be for a different delete string, in same bin only because of hash collision
					|| (suggestionLen == candidateLen && suggestion != candidate))
					// if sugg len = delete len, then it either equals delete or is in same bin only because of hash collision
				{
					continue;
				}

				auto suggestPrefixLen = std::min(suggestionLen, _prefixLength);
				if ((suggestPrefixLen > inputPrefixLen)
					&& (static_cast<int>(suggestPrefixLen - candidateLen) > maxEditDistance2))
				{
					continue;
				}
				//True Damerau-Levenshtein Edit Distance: adjust distance, if both distances>0
				//We allow simultaneous edits (deletes) of maxEditDistance on on both the dictionary and the input term. 
				//For replaces and adjacent transposes the resulting edit distance stays <= maxEditDistance.
				//For inserts and deletes the resulting edit distance might exceed maxEditDistance.
				//To prevent suggestions of a higher edit distance, we need to calculate the resulting edit distance, if there are simultaneous edits on both sides.
				//Example: (bank==bnak and bank==bink, but bank!=kanb and bank!=xban and bank!=baxn for maxEditDistance=1)
				//Two deletes on each side of a pair makes them all equal, but the first two pairs have edit distance=1, the others edit distance=2.
				int distance = 0;
				int minLen = 0;
				if (candidateLen == 0)
				{
					//suggestions which have no common chars with input (inputLen<=maxEditDistance && suggestionLen<=maxEditDistance)
					distance = static_cast<int>(std::max(inputLen, suggestionLen));
					auto flag = hashset2.insert(suggestion);
					if (distance > maxEditDistance2 || !flag.second)
					{
						continue;
					}
				}
				else if (suggestionLen == 1)
				{
					// not entirely sure what happens here yet
					if (input.find(suggestion[0]) == std::string_view::npos)
					{
						distance = static_cast<int>(inputLen);
					}
					else
					{
						distance = static_cast<int>(inputLen) - 1;
					}
					auto flag = hashset2.insert(suggestion);
					if (distance > maxEditDistance2 || !flag.second)
					{
						continue;
					}
				}
				else
				{
					// number of edits in prefix == maxediddistance  AND no identic suffix
					// then editdistance>maxEditDistance and no need for Levenshtein calculation  
					if (_prefixLength - 1 == candidateLen)
					{
						minLen = static_cast<int>(std::min(inputLen, suggestionLen) - _prefixLength);
						if (minLen > 1 && input.substr(inputLen + 1 - minLen) != suggestion.substr(
							suggestionLen + 1 - minLen))
						{
							continue;
						}

						if (minLen > 0
							&& (input[inputLen - minLen] != suggestion[suggestionLen - minLen])
							&& ((input[inputLen - minLen - 1] != suggestion[suggestionLen - minLen])
								|| (input[inputLen - minLen] != suggestion[suggestionLen - minLen - 1])))
						{
							continue;
						}
					}

					// DeleteInSuggestionPrefix is somewhat expensive, and only pays off when verbosity is Top or Closest.
					if ((!DeleteInSuggestionPrefix(candidate, candidateLen, suggestion, suggestionLen))
						|| !hashset2.insert(suggestion).second)
					{
						continue;
					}
					distance = _editDistance.Compare(input, suggestion, maxEditDistance2);
					if (distance < 0)
					{
						continue;
					}
				}

				//save some time
				//do not process higher distances than those already found, if verbosity<All (note: maxEditDistance2 will always equal maxEditDistance when Verbosity.All)
				if (distance <= maxEditDistance2)
				{
					suggestionCount = _words[suggestion];
					SuggestItem si(suggestion, distance, suggestionCount);

					if (!suggestions.empty() && distance < maxEditDistance2)
					{
						suggestions.clear();
					}

					maxEditDistance2 = distance;
					suggestions.push_back(si);
				}
			}
		}

		//add edits 
		//derive edits (deletes) from candidate (input) and add them to candidates list
		//this is a recursive process until the maximum edit distance has been reached
		if ((lengthDiff < maxEditDistance) && (candidateLen <= _prefixLength))
		{
			//save some time
			//do not create edits with edit distance smaller than suggestions already found
			if (lengthDiff >= maxEditDistance2)
			{
				continue;
			}
			for (std::size_t i = 0; i < candidateLen; i++)
			{
				std::string temp(candidate);
				std::string del = temp.erase(i, 1);

				if (hashset1.insert(del).second)
				{
					candidates.push_back(del);
				}
			}
		}
	}

	std::sort(suggestions.begin(), suggestions.end(), SuggestItem::Comapare);

	return suggestions;
}

bool SymSpell::BuildDeletesWords(const std::string& key)
{
	//edits/suggestions are created only once, no matter how often word occurs
	//edits/suggestions are created only as soon as the word occurs in the corpus, 
	//even if the same term existed before in the dictionary as an edit from another word
	if (key.size() > _maxDictionaryWordLength)
	{
		_maxDictionaryWordLength = key.size();
	}
	//create deletes
	auto edits = EditsPrefix(key);
	// �����������д��
	for (auto it = edits.begin(); it != edits.end(); ++it)
	{
		int deleteHash = GetStringHash(*it);
		auto deletesFounded = _deletes.find(deleteHash);
		if (deletesFounded != _deletes.end())
		{
			auto& suggestions = deletesFounded->second;
			suggestions.emplace_back(key);
		}
		else
		{
			std::vector<std::string> suggestions = {key};

			_deletes.insert({deleteHash, suggestions});
		}
	}
	return true;
}

bool SymSpell::BuildAllDeletesWords()
{
	bool ret = true;
	for(auto& pair: _words)
	{
		ret &= BuildDeletesWords(pair.first);
	}
	return ret;
}

std::unordered_set<std::string> SymSpell::EditsPrefix(std::string_view key)
{
	std::unordered_set<std::string> hashSet;
	if (key.size() <= static_cast<std::size_t>(_maxDictionaryEditDistance))
	{
		hashSet.insert("");
	}
	if (key.size() > _prefixLength)
	{
		key = key.substr(0, _prefixLength);
	}
	hashSet.insert(std::string(key));
	Edits(key, 0, hashSet);

	return hashSet;
}

void SymSpell::Edits(std::string_view word, int editDistance, std::unordered_set<std::string>& deleteWord)
{
	editDistance++;
	if (word.size() > 1)
	{
		for (std::size_t i = 0; i < word.size(); i++)
		{
			std::string tmp(word);
			auto del = tmp.erase(i, 1);
			if (deleteWord.insert(del).second)
			{
				if (editDistance < _maxDictionaryEditDistance)
				{
					Edits(del, editDistance, deleteWord);
				}
			}
		}
	}
}

int SymSpell::GetStringHash(std::string_view source)
{
	uint32_t lenMask = static_cast<uint32_t>(source.size());
	if (lenMask > 3)
	{
		lenMask = 3;
	}
	uint32_t hash = 2166136261;
	for (auto c : source)
	{
		//unchecked, its fine even if it can be overflowed
		hash ^= c;
		hash *= 16777619;
	}

	hash &= _compactMask;
	hash |= lenMask;
	return static_cast<int>(hash);
}

bool SymSpell::DeleteInSuggestionPrefix(std::string_view deleteSuggest, std::size_t deleteLen,
                                        std::string_view suggestion,
                                        std::size_t suggestionLen)
{
	if (deleteLen == 0)
	{
		return true;
	}
	if (_prefixLength < suggestionLen)
	{
		suggestionLen = _prefixLength;
	}

	std::size_t j = 0;
	for (std::size_t i = 0; i < deleteLen; i++)
	{
		char delChar = deleteSuggest[i];
		while (j < suggestionLen && delChar != suggestion[j]) { j++; }
		if (j == suggestionLen)
		{
			return false;
		}
	}
	return true;
}
