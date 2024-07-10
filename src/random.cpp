#include <random>

#include "random.h"

float randFloat(float LO, float HI) {
    return LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));;
}