#pragma once

#include <string>
#include <string_view>
#include <vector>

class FunctionOption {
public:
    void Parse(std::string_view option);

    std::string &GetKey();

    std::string GetParam(std::size_t index);

    std::size_t GetParamSize();

private:
    std::string _key;
    std::vector<std::string> _params;
};
