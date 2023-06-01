#pragma once

#include <string_view>
#include <memory>
#include "IDistance.h"

class EditDistance
{
public:
	explicit EditDistance();

	int Compare(std::string_view lhs, std::string_view rhs, std::size_t maxEditDistance);
	
private:
	std::shared_ptr<IDistance> _distanceAlgorithm;
};
