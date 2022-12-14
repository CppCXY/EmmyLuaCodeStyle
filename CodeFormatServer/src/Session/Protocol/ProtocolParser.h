#pragma once

#include <cstdlib>
#include <string_view>
#include <memory>
#include <variant>
#include "LSP/LSP.h"

class ProtocolParser
{
public:
	ProtocolParser();

	void Parse(std::string_view msg);

	nlohmann::json GetParams();

	std::string_view GetMethod();

	std::string SerializeProtocol(std::shared_ptr<lsp::Serializable> result);
private:
	std::variant<int, std::string, void*> _id;
	std::string _method;
	nlohmann::json _params;
};
