#pragma once

#include "src/objects/object.h"

class ComponentBase {
public:
    ComponentBase(Object &self): _self(self) {};
    ComponentBase(const Object &self): _self(self) {};

    Object getObject() { return _self; };

protected:
    Object _self;
};
