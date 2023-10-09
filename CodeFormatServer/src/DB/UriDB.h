#pragma once

#include "DBBase.h"
#include <map>
#include <string>
#include <vector>

class UriDB : public DBBase<std::string, std::size_t,
                            std::map<std::string, std::size_t, std::less<>>> {
public:
    UriDB();
};
