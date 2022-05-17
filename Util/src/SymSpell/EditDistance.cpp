#include "Util/SymSpell/EditDistance.h"
#include "DamerauOSADistance.hpp"

EditDistance::EditDistance()
	: _distanceAlgorithm(std::make_shared<DamerauOSA>())
{
}

int EditDistance::Compare(std::string_view lhs, std::string_view rhs, std::size_t maxEditDistance)
{
	return _distanceAlgorithm->Distance(lhs, rhs, maxEditDistance);
}
