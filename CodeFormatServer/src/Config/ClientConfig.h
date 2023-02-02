#pragma once
#include "LSP/LSP.h"

class ClientConfig : lsp::Serializable {
public:
   bool emmylua_lint_codeStyle = true;
   bool emmylua_lint_nameStyle = false;
   bool emmylua_spell_enable = false;
   std::vector<std::string> emmylua_spell_dict;

   void Deserialize(nlohmann::json json) override;
};
