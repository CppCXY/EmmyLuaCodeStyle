#include "UriDB.h"

UriDB::UriDB()
        : DBBase<std::string, std::size_t, std::map<std::string, std::size_t, std::less<>>>() {

}
