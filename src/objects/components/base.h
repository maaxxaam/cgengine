#pragma once

#include "src/objects/object.h"

class ComponentBase {
public:
    ComponentBase(const Object &self): _self(self) {};

    Object getObject() { return _self; };

protected:
    Object _self;
};
