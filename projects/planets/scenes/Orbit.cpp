#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Orbit.h"

Orbit::Orbit(TransformComponent* origin, glm::vec2 orbitSpeed, float orbitDistance, glm::vec2 spinSpeed) {
    _orbitOrigin = origin;
    _distance = orbitDistance;
    _orbitSpeed = orbitSpeed;
    _orbit = glm::zero<glm::vec2>();
    _spin = glm::zero<glm::vec2>();
    _spinSpeed = spinSpeed;

    calculate();
}

void Orbit::calculate() {

}

MaybeError Orbit::update(float delta) {
    return std::nullopt;
}

void Orbit::applyTo(TransformComponent* transform) {
    transform->setMatrix(getMatrix());
}

glm::mat4 Orbit::dqtom4(glm::dualquat dq) {
    glm::mat4 out;
    out[0][0] = 1.0 - (2.0 * dq[0][1] * dq[0][1]) - (2.0 * dq[0][2] * dq[0][2]);
    out[0][1] = (2.0 * dq[0][0] * dq[0][1]) + (2.0 * dq[0][3] * dq[0][2]);
    out[0][2] = (2.0 * dq[0][0] * dq[0][2]) - (2.0 * dq[0][3] * dq[0][1]);
    out[0][3] = 0;
    out[1][0] = (2.0 * dq[0][0] * dq[0][1]) - (2.0 * dq[0][3] * dq[0][2]);
    out[1][1] = 1.0 - (2.0 * dq[0][0] * dq[0][0]) - (2.0 * dq[0][2] * dq[0][2]);
    out[1][2] = (2.0 * dq[0][1] * dq[0][2]) + (2.0 * dq[0][3] * dq[0][0]);
    out[1][3] = 0;
    out[2][0] = (2.0 * dq[0][0] * dq[0][2]) + (2.0 * dq[0][3] * dq[0][1]);
    out[2][1] = (2.0 * dq[0][1] * dq[0][2]) - (2.0 * dq[0][3] * dq[0][0]);
    out[2][2] = 1.0 - (2.0 * dq[0][0] * dq[0][0]) - (2.0 * dq[0][1] * dq[0][1]);
    out[2][3] = 0;
    out[3][0] = 2.0 * (-dq[1][3] * dq[0][0] + dq[1][0] * dq[0][3] - dq[1][1] * dq[0][2] + dq[1][2] * dq[0][1]);
    out[3][1] = 2.0 * (-dq[1][3] * dq[0][1] + dq[1][0] * dq[0][2] + dq[1][1] * dq[0][3] - dq[1][2] * dq[0][0]);
    out[3][2] = 2.0 * (-dq[1][3] * dq[0][2] - dq[1][0] * dq[0][1] + dq[1][1] * dq[0][0] + dq[1][2] * dq[0][3]);
    out[3][3] = 1;
    return out;
}

glm::mat4 Orbit::getMatrix() {
    // glm::mat4 translation = glm::translate(glm::mat4{1.0f}, _orbitOrigin->getTranslation());
    glm::mat4 translation = glm::translate(glm::mat4{1.0f}, glm::vec3(1.f));
    // glm::mat4 rotation = dqtom4(_total);
    // return translation * rotation;
    return translation;
}