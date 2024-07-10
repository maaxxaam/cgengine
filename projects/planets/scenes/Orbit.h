#pragma once

#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "src/objects/components/base.h"
#include "src/objects/components/transform.h"
#include "src/update/update.h"

class Orbit: public Update, public ComponentBase {
public:
    Orbit(const Object &self, watch_ptr<TransformComponent> origin, glm::vec3 orbit, float orbitSpeed, float orbitDistance, glm::vec3 spin, float spinSpeed);

    glm::mat4 getMatrix();

    void applyTo(watch_ptr<TransformComponent> transform);

    MaybeError update(float delta) override;
private:
    void calculate();
    glm::mat4 dqtom4(glm::dualquat dq);

    watch_ptr<TransformComponent> _orbitOrigin;
    glm::vec3 _orbitAxis;
    float _orbitPos;
    float _orbitSpeed;
    glm::quat _orbit;
    float _distance;
    glm::vec3 _spinAxis;
    float _spinPos;
    float _spinSpeed;
    glm::quat _spin;
    glm::mat4 _total;
};
