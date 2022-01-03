#pragma once

#include <memory>
#include "Util/Nonmoveable.h"
#include "Util/Noncopyable.h"
#include "ServiceType.h"
#include "CodeFormatServer/LanguageClient.h"

class Service : public Noncopyable,
                public Nonmoveable
{
public:
	LANGUAGE_SERVICE(Service);

	explicit Service(std::shared_ptr<LanguageClient> owner);

	virtual ~Service() = default;

	virtual bool Initialize() { return true; }

	virtual void Start() {};

	template <ServiceClass T>
	std::shared_ptr<T> GetService() { return _owner->GetService<T>(); }
protected:
	std::shared_ptr<LanguageClient> _owner;
};
