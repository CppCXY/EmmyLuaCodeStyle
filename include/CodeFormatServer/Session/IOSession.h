#pragma once
#include <string>
#include "CodeFormatServer/LanguageService.h"

class IOSession
{
public:
	IOSession();
	virtual ~IOSession();

	virtual void Run() = 0;
	// virtual void Write();
protected:
	std::string Handle(std::string_view msg);
private:
	LanguageService _service;
};
