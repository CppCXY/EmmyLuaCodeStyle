#pragma once

enum class ServiceType
{
	Service = -1,
	FormatService = 0,
//	SpellService,
//	CommandService,
//	CodeActionService,

	ServiceCount 
};

#define LANGUAGE_SERVICE(ServiceClass)\
    inline static constexpr ServiceType ServiceIndex = ServiceType::ServiceClass
