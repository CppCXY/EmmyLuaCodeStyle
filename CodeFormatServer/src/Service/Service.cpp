#include "CodeFormatServer/Service/Service.h"

Service::Service(std::shared_ptr<LanguageClient> owner)
	: _owner(owner)
{
}
