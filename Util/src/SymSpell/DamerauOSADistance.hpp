#pragma once

#include <vector>
#include "Util/SymSpell/IDistance.h"

/// <summary>
/// Class providing optimized methods for computing Damerau-Levenshtein Optimal string
/// Alignment (OSA) comparisons between two strings.
/// </summary>
/// <remarks>
/// Copyright ©2015-2018 SoftWx, Inc.
/// The inspiration for creating highly optimized edit distance functions was 
/// from Sten Hjelmqvist's "Fast, memory efficient" algorithm, described at
/// http://www.codeproject.com/Articles/13525/Fast-memory-efficient-Levenshtein-algorithm
/// The Damerau-Levenshtein algorithm is basically the Levenshtein algorithm with a 
/// modification that considers transposition of two adjacent characters as a single edit.
/// The optimized algorithm was described in detail in my post at 
/// http://blog.softwx.net/2015/01/optimizing-damerau-levenshtein_15.html
/// Also see http://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance
/// Note that this implementation of Damerau-Levenshtein is the simpler and faster optimal
/// string alignment (aka restricted edit) distance that difers slightly from the classic
/// algorithm by imposing the restriction that no substring is edited more than once. So,
/// for example, "CA" to "ABC" has an edit distance of 2 by a complete application of
/// Damerau-Levenshtein, but has a distance of 3 by the method implemented here, that uses
/// the optimal string alignment algorithm. This means that this algorithm is not a true
/// metric since it does not uphold the triangle inequality. In real use though, this OSA
/// version may be desired. Besides being faster, it does not give the lower distance score
/// for transpositions that occur across long distances. Actual human error transpositions
/// are most likely for adjacent characters. For example, the classic Damerau algorithm 
/// gives a distance of 1 for these two strings: "sated" and "dates" (it counts the 's' and
/// 'd' as a single transposition. The optimal string alignment version of Damerau in this
/// class gives a distance of 2 for these two strings (2 substitutions), as it only counts
/// transpositions for adjacent characters.
/// The methods in this class are not threadsafe. Use the static versions in the Distance
/// class if that is required.</remarks>
///	@CppCXY : 同样的我修改了全部实现, 删掉了不用的实现, 我不理解为什么distance会返回double
class DamerauOSA : public IDistance
{
public:
	/// <summary>Create a new instance of DamerauOSA.</summary>
	DamerauOSA() = default;

	/// <summary>Create a new instance of DamerauOSA using the specified expected
	/// maximum string length that will be encountered.</summary>
	/// <remarks>By specifying the max expected string length, better memory efficiency
	/// can be achieved.</remarks>
	/// <param name="expectedMaxStringLength">The expected maximum length of strings that will
	/// be passed to the edit distance functions.</param>
	DamerauOSA(std::size_t expectedMaxStringLength)
		: _baseChar1Costs(expectedMaxStringLength, 0),
		  _basePrevChar1Costs(expectedMaxStringLength, 0)
	{
	}

	/// <summary>Compute and return the Damerau-Levenshtein optimal string
	/// alignment edit distance between two strings.</summary>
	/// <remarks>https://github.com/softwx/SoftWx.Match
	/// This method is not threadsafe.</remarks>
	/// <param name="string1">One of the strings to compare.</param>
	/// <param name="string2">The other string to compare.</param>
	/// <returns>0 if the strings are equivalent, otherwise a positive number whose
	/// magnitude increases as difference between the strings increases.</returns>
	int Distance(std::string_view string1, std::string_view string2) override
	{
		if (string1.empty())
		{
			return static_cast<int>(string2.size());
		}
		if (string2.empty())
		{
			return static_cast<int>(string1.size());
		}

		// if strings of different lengths, ensure shorter string is in string1. This can result in a little
		// faster speed by spending more time spinning just the inner loop during the main processing.
		if (string1.size() > string2.size())
		{
			std::swap(string1, string2);
		}

		// identify common suffix and/or prefix that can be ignored
		int len1 = 0, len2 = 0, start = 0;
		PrefixSuffixPrep(string1, string2, len1, len2, start);
		if (len1 == 0)
		{
			return len2;
		}

		if (len2 > static_cast<int>(this->_baseChar1Costs.size()))
		{
			_baseChar1Costs.resize(len2, 0);
			_basePrevChar1Costs.resize(len2, 0);
		}
		return Distance(string1, string2, len1, len2, start, _baseChar1Costs, _basePrevChar1Costs);
	}

	int Distance(std::string_view string1, std::string_view string2, std::size_t maxEditDistance) override
	{
		if (string1.empty() || string2.empty())
		{
			return NullDistanceResults(string1, string2, maxEditDistance);
		}
		if (maxEditDistance <= 0)
		{
			return (string1 == string2) ? 0 : -1;
		}
		if (string1.size() > string2.size())
		{
			std::swap(string1, string2);
		}
		if (string2.size() - string1.size() > maxEditDistance)
		{
			return -1;
		}

		int len1 = 0, len2 = 0, start = 0;
		PrefixSuffixPrep(string1, string2, len1, len2, start);
		if (len1 == 0)
		{
			return (len2 <= static_cast<int>(maxEditDistance)) ? len2 : -1;
		}
		if (len2 > static_cast<int>(_baseChar1Costs.size()))
		{
			_baseChar1Costs.resize(len2, 0);
			_basePrevChar1Costs.resize(len2, 0);
		}

		if (static_cast<int>(maxEditDistance) < len2)
		{
			return Distance(string1, string2, len1, len2, start, static_cast<int>(maxEditDistance), _baseChar1Costs, _basePrevChar1Costs);
		}

		return Distance(string1, string2, len1, len2, start, _baseChar1Costs, _basePrevChar1Costs);
	}

	/// <summary>Internal implementation of the core Damerau-Levenshtein, optimal string alignment algorithm.</summary>
	/// <remarks>https://github.com/softwx/SoftWx.Match</remarks>
	static int Distance(std::string_view string1, std::string_view string2, int len1, int len2, int start,
	                    std::vector<int> char1Costs,
	                    std::vector<int> prevChar1Costs)
	{
		int j;
		for (j = 0; j < len2; j++) char1Costs[j] = j + 1;
		char char1 = ' ';
		int currentCost = 0;
		for (int i = 0; i < len1; ++i)
		{
			char prevChar1 = char1;
			char1 = string1[start + i];
			char char2 = ' ';
			int leftCharCost = i, aboveCharCost = i;
			int nextTransCost = 0;
			for (j = 0; j < len2; ++j)
			{
				int thisTransCost = nextTransCost;
				nextTransCost = prevChar1Costs[j];
				prevChar1Costs[j] = currentCost = leftCharCost; // cost of diagonal (substitution)
				leftCharCost = char1Costs[j]; // left now equals current cost (which will be diagonal at next iteration)
				char prevChar2 = char2;
				char2 = string2[start + j];
				if (char1 != char2)
				{
					//substitution if neither of two conditions below
					if (aboveCharCost < currentCost) currentCost = aboveCharCost; // deletion
					if (leftCharCost < currentCost) currentCost = leftCharCost; // insertion
					++currentCost;
					if ((i != 0) && (j != 0)
						&& (char1 == prevChar2)
						&& (prevChar1 == char2)
						&& (thisTransCost + 1 < currentCost))
					{
						currentCost = thisTransCost + 1; // transposition
					}
				}
				char1Costs[j] = aboveCharCost = currentCost;
			}
		}
		return currentCost;
	}

	// <summary>Internal implementation of the core Damerau-Levenshtein, optimal string alignment algorithm
	/// that accepts a maxDistance.</summary>
	static int Distance(std::string_view string1, std::string_view string2, int len1, int len2, int start,
	                    int maxDistance, std::vector<int> char1Costs, std::vector<int> prevChar1Costs)
	{
		int i, j;
		//for (j = 0; j < maxDistance;) char1Costs[j] = ++j;
		for (j = 0; j < maxDistance; j++)
			char1Costs[j] = j + 1;
		for (; j < len2;) char1Costs[j++] = maxDistance + 1;
		int lenDiff = len2 - len1;
		int jStartOffset = maxDistance - lenDiff;
		int jStart = 0;
		int jEnd = maxDistance;
		char char1 = ' ';
		int currentCost = 0;
		for (i = 0; i < len1; ++i)
		{
			char prevChar1 = char1;
			char1 = string1[start + i];
			char char2 = ' ';
			int leftCharCost, aboveCharCost;
			leftCharCost = aboveCharCost = i;
			int nextTransCost = 0;
			// no need to look beyond window of lower right diagonal - maxDistance cells (lower right diag is i - lenDiff)
			// and the upper left diagonal + maxDistance cells (upper left is i)
			jStart += (i > jStartOffset) ? 1 : 0;
			jEnd += (jEnd < len2) ? 1 : 0;
			for (j = jStart; j < jEnd; ++j)
			{
				int thisTransCost = nextTransCost;
				nextTransCost = prevChar1Costs[j];
				prevChar1Costs[j] = currentCost = leftCharCost; // cost on diagonal (substitution)
				leftCharCost = char1Costs[j]; // left now equals current cost (which will be diagonal at next iteration)
				char prevChar2 = char2;
				char2 = string2[start + j];
				if (char1 != char2)
				{
					// substitution if neither of two conditions below
					if (aboveCharCost < currentCost) currentCost = aboveCharCost; // deletion
					if (leftCharCost < currentCost) currentCost = leftCharCost; // insertion
					++currentCost;
					if ((i != 0) && (j != 0)
						&& (char1 == prevChar2)
						&& (prevChar1 == char2)
						&& (thisTransCost + 1 < currentCost))
					{
						currentCost = thisTransCost + 1; // transposition
					}
				}
				char1Costs[j] = aboveCharCost = currentCost;
			}
			if (char1Costs[i + lenDiff] > maxDistance) return -1;
		}
		return (currentCost <= maxDistance) ? currentCost : -1;
	}

	/// <summary>Calculates starting position and lengths of two strings such that common
	/// prefix and suffix substrings are excluded.</summary>
	/// <remarks>Expects string1.size() to be less than or equal to string2.size()</remarks>
	static void PrefixSuffixPrep(std::string_view string1, std::string_view string2, int& len1, int& len2, int& start)
	{
		len1 = static_cast<int>(string1.size()); // this is also the minimum length of the two strings
		len2 = static_cast<int>(string2.size());

		// suffix common to both strings can be ignored
		while (len1 != 0 && string1[len1 - 1] == string2[len2 - 1])
		{
			len1--;
			len2--;
		}
		// prefix common to both strings can be ignored
		start = 0;
		while (start != len1 && string1[start] == string2[start])
		{
			start++;
		}
		if (start != 0)
		{
			len2 -= start; // length of the part excluding common prefix and suffix
			len1 -= start;
		}
	}

	static int NullDistanceResults(std::string_view string1, std::string_view string2, std::size_t maxEditDistance)
	{
		if (string1.empty())
			return (string2.empty()) ? 0 : (string2.size() <= maxEditDistance) ? static_cast<int>(string2.size()) : -1;
		return (string1.size() <= maxEditDistance) ? static_cast<int>(string1.size()) : -1;
	}

private:
	std::vector<int> _baseChar1Costs;
	std::vector<int> _basePrevChar1Costs;
};
