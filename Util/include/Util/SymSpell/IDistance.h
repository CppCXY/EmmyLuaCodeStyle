#pragma once
#include <string_view>

class IDistance
{
public:
	virtual ~IDistance() {};

	virtual int Distance(std::string_view lhs, std::string_view rhs) = 0;

	virtual int Distance(std::string_view lhs, std::string_view rhs, std::size_t maxEditDistance) = 0;
};