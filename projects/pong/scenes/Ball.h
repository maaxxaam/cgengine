#pragma once

#include "src/objects/components/rigidbody.h"
#include "src/update/update.h"

class Ball: public Update {
public:
    Ball() {};

    void setBall(RigidBodyComponent* ball);
    MaybeError update(float delta) override;
    void setActive(bool active) { _active = active; };
    void reset();
private:
    float _movementFactor = 5.f;
    bool _active = true;
    RigidBodyComponent* _body;
};
