#include "Ball.h"

void Ball::setBall(RigidBodyComponent* ball) {
    _body = ball;
    _body->setFriction(0.f);
    _body->setRestitution(1.f);
}

MaybeError Ball::update(float delta) {
    if (_body == nullptr) return std::nullopt;
    auto movement = glm::normalize(_body->GetVelocity()) * _movementFactor;
    if (_active)
        _body->setVelocity(glm::vec3(movement.x, movement.y, movement.z));
    else
        _body->setVelocity(glm::vec3(0.f, 0.f, 0.f));
    return std::nullopt;
}

void Ball::reset() {
    _body->setPosition(glm::vec3(0.f, 0.f, -5.f));
    _body->applyCentralImpulse(glm::vec3(0.f, 0.f, -1.f));
}
