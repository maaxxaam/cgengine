#pragma once

#include "base.h"

class SSBOIndex: public ComponentBase {
public:
	SSBOIndex(const Object &self, int _index): ComponentBase(self), index(_index) {};
	int index;
};
