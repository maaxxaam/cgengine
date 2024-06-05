#pragma once

#include <string>

#include "src/objects/object.h"
#include "src/objects/components/base.h"

class TagComponent: public ComponentBase {
public:
    TagComponent(const Object& self, const std::string name): ComponentBase(self), _name(name) {};

    std::string getName() const { return _name; };
private:
    std::string _name;
};
