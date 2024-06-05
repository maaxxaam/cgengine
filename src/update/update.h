#pragma once

#include "src/error.h"

class Update {
public:
    virtual MaybeError update(float delta) = 0;
};