#pragma once

enum class ServiceType
{
	Service = -1,
	FormatService = 0,
//	SpellService,
//	CommandService,
//	CodeActionService,

    ConfigService,

	ServiceCount 
};

#define LANGUAGE_SERVICE(ServiceClass)\
    inline static constexpr ServiceType ServiceIndex = ServiceType::ServiceClass
