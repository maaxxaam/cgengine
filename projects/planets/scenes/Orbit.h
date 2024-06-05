#pragma once

#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "src/objects/components/transform.h"
#include "src/update/update.h"

class Orbit: public Update {
public:
    Orbit(TransformComponent* origin, glm::vec2 orbitSpeed, float orbitDistance, glm::vec2 spinSpeed);

    glm::mat4 getMatrix();

    void applyTo(TransformComponent* transform);

    MaybeError update(float delta) override;
private:
    void calculate();
    glm::mat4 dqtom4(glm::dualquat dq);

    TransformComponent* _orbitOrigin;
    glm::vec2 _orbit, _orbitSpeed;
    float _distance;
    glm::vec2 _spin, _spinSpeed;
    glm::mat4 _total;
};
