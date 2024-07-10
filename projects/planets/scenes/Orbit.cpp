#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Orbit.h"

Orbit::Orbit(const Object &self, watch_ptr<TransformComponent> origin, glm::vec3 orbit, float orbitSpeed, float orbitDistance, glm::vec3 spin, float spinSpeed):
    ComponentBase(self) {
    _orbitOrigin = origin;
    _orbitAxis.x = _orbitAxis.x == 0.f ? 0.01f: _orbitAxis.x;
    _orbitAxis.y = _orbitAxis.y == 0.f ? 0.01f: _orbitAxis.y;
    _orbitAxis = glm::normalize(orbit);
    _distance = orbitDistance;
    _orbitSpeed = orbitSpeed;
    _spinSpeed = spinSpeed;
    _spinAxis = glm::normalize(spin);
    _orbit = glm::angleAxis(0.f, orbit);
    _spin = glm::angleAxis(0.f, spin);
    _spinPos = 0.f;
    _orbitPos = 0.f;

    calculate();
}

void Orbit::calculate() {
    // glm::mat4 a = glm::translate(glm::mat4(1.f), glm::vec3(1.f, 0.f, 0.f) * _distance) * glm::toMat4(_orbit);
    glm::vec3 b = /*glm::vec3(1.f, 0.f, 0.f)*/ glm::cross(_orbitAxis, _spinAxis) * _distance * _orbit;
    // glm::vec2 orbitYP{glm::yaw(_orbit), glm::pitch(_orbit)};
    // fmt::println("Orbit: {} {}", orbitYP.x, orbitYP.y);
    // glm::vec3 orbitV = glm::normalize(glm::vec3(
		// cos(orbitYP.y) * cos(orbitYP.x),
		// sin(orbitYP.y),
		// sin(orbitYP.x) * cos(orbitYP.y))) * _distance;
    // glm::vec3 translation = orbitV + _orbitOrigin->getTranslation();
    glm::vec3 translation = b + _orbitOrigin->getTranslation();
    // glm::mat4 translation = a + glm::translate(glm::mat4{1.f}, _orbitOrigin->getTranslation());
    _total = glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(_spin);
    // _total = translation * glm::toMat4(_spin);
}

MaybeError Orbit::update(float delta) {
    _orbitPos += _orbitSpeed * delta;
    _spinPos += _spinSpeed * delta;
    const float pi = glm::pi<float>();
    _orbitPos = glm::mod(_orbitPos, pi * 2);
    _spinPos = glm::mod(_spinPos, pi * 2);
    _orbit = glm::angleAxis(_orbitPos, _orbitAxis);
    _spin = glm::angleAxis(_spinPos, _spinAxis);
    // fmt::println("Orbit: {} {}", _orbitPos, _spinPos);

    calculate();
    return std::nullopt;
}

void Orbit::applyTo(watch_ptr<TransformComponent> transform) {
    transform->setMatrix(_total);
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
    // glm::mat4 translation = glm::translate(glm::mat4{1.0f}, glm::vec3(1.f));
    // glm::mat4 rotation = dqtom4(_total);
    // return translation * rotation;
    return _total;
}