#include "LuaParser/LuaParseException.h"

LuaParseException::LuaParseException(std::string_view message)
	: runtime_error(std::string(message))
{
}
