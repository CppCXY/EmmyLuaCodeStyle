#pragma once

#include "ServiceType.h"
#include <memory>

class LanguageServer;

class Service {
public:
    LANGUAGE_SERVICE(Service);

    explicit Service(LanguageServer *owner);

    virtual ~Service() = default;

    virtual bool Initialize() { return true; }

    virtual void Start(){};

protected:
    LanguageServer *_owner;
};
