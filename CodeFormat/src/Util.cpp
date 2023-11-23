#include "Util.h"
#include <fstream>
#include <sstream>


std::optional<std::string> ReadFile(std::string_view path) {
    std::string newPath(path);
#ifdef _WIN32
    std::fstream fin(newPath, std::ios::in | std::ios::binary);
#else
    std::fstream fin(newPath, std::ios::in);
#endif

    if (fin.is_open()) {
        std::stringstream s;
        s << fin.rdbuf();
        return std::move(s.str());
    }

    return std::nullopt;
}