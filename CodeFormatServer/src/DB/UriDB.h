#pragma once

#include "DBBase.h"
#include <string>
#include <vector>
#include <map>

class UriDB :
        public DBBase<std::string, std::size_t,
                std::map<std::string, std::size_t, std::less<>>> {
public:
    UriDB();
};

