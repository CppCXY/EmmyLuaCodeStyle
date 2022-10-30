#include "LuaParser/exception/LuaParseException.h"
#include <string>


LuaParseException::LuaParseException(std::string_view message)
	: runtime_error(std::string(message))
{
}
