#pragma once

#include "IDistance.h"
#include <memory>
#include <string_view>

class EditDistance {
public:
    explicit EditDistance();

    int Compare(std::string_view lhs, std::string_view rhs, std::size_t maxEditDistance);

private:
    std::shared_ptr<IDistance> _distanceAlgorithm;
};
