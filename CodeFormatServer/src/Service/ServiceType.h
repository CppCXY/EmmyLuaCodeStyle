#pragma once

enum class ServiceType
{
	Service = -1,
	FormatService = 0,
	DiagnosticService,
	CommandService,
	CodeActionService,

    ConfigService,

	ServiceCount 
};

#define LANGUAGE_SERVICE(ServiceClass)\
    inline static constexpr ServiceType ServiceIndex = ServiceType::ServiceClass
