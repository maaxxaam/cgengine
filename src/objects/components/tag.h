#pragma once

#include "eastloverloads.h"
#include <EASTL/string.h>
#include <EASTL/fixed_substring.h>

#include "src/objects/object.h"
#include "src/objects/components/base.h"

struct TagComponent: public ComponentBase {
    TagComponent(const Object& self, const eastl::string name): ComponentBase(self), _name(name) {};

    const eastl::fixed_substring<char> getName() const { return _name; };
private:
    eastl::string _name;
};
