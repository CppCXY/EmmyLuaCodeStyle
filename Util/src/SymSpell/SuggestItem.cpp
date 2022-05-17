#include "Util/SymSpell/SuggestItem.h"

bool SuggestItem::Comapare(const SuggestItem& lhs, const SuggestItem& rhs)
{
	return rhs.Count < lhs.Count;
}

SuggestItem::SuggestItem()
{
}

SuggestItem::SuggestItem(std::string term, int distance, int count)
	: Term(term),
	  Distance(distance),
	  Count(count)
{
}
