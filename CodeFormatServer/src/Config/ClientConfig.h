#pragma once
#include "LSP/LSP.h"
#include "Util/InfoTree/InfoTree.h"

class ClientConfig : lsp::Serializable {
public:
    std::vector<std::string> emmylua_spell_dict;
    InfoTree configTree;

    void Deserialize(nlohmann::json json) override;
};
