#include "Follow.h"

Follow::Follow(Object &self, watch_ptr<TransformComponent> target): 
    ComponentBase(self), _target(target) {
    _follower = self.getComponent<TransformComponent>();
    glm::vec3 youPos = _follower->getTranslation();
    glm::vec3 tPos = _target->getTranslation();
    _offsetPos = youPos - tPos;
    glm::vec3 youPYR = _follower->getEulerOrientation();
    glm::vec3 tPYR = _target->getEulerOrientation();
    _offsetPYR = youPYR - tPYR;
}

MaybeError Follow::update(float delta) {
    glm::vec3 tPYR = _target->getEulerOrientation();
    glm::vec3 tPos = _target->getTranslation();
    glm::quat q = _target->getOrientation();
    glm::vec3 rotated = _offsetPos * q;
    glm::vec3 youPos = tPos + rotated;
    glm::vec3 youPYR = tPYR + _offsetPYR;
    _follower->setEulerOrientation(youPYR);
    _follower->setTranslation(youPos);
    return std::nullopt;
}