#include "CodeService/LuaLinterOptions.h"
#include "nlohmann/json.hpp"

void LuaLinterOptions::ParseFromJson(nlohmann::json json)
{
	if(json.is_object())
	{
		if(json["max-line-width"].is_number_integer())
		{
			max_line_length.Deserialize(json["max-line-width"]);
		}

		if(!json["indent-style"].is_null())
		{
			indent_style.Deserialize(json["indent-style"]);
		}

		if(!json["quote-style"].is_null())
		{
			quote_style.Deserialize(json["quote-style"]);
		}

		if(!json["white-space"].is_null())
		{
			white_space.Deserialize(json["white-space"]);
		}

		if(!json["align-style"].is_null())
		{
			align_style.Deserialize(json["align-style"]);
		}

		if(!json["line-space"].is_null())
		{
			line_space.Deserialize(json["line-space"]);
		}

		if(!json["name-style"].is_null())
		{
			name_style.Deserialize(json["name-style"]);
		}

	}
}
