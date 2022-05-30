#pragma once

enum class ServiceType
{
	Service = -1,
	CodeFormatService = 0,
	ModuleService = 1,
	CompletionService = 2,
	CommandService = 3,
	CodeActionService = 4,

	ServiceCount 
};

#define LANGUAGE_SERVICE(ServiceClass)\
    inline static constexpr ServiceType ServiceIndex = ServiceType::ServiceClass
