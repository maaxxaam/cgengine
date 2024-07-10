#pragma once

#include "singleton.h"

float randFloat(float LO, float HI);

class Random: public Singleton<Random> {
public:
    float randFloat(float LO, float HI);
private:
    //
};

#define RandomS Random::instance()
