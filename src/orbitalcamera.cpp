#include "orbitalcamera.h"

OrbitalCamera::OrbitalCamera(const glm::vec3 *target, Camera *camera): CameraController(camera) {
    _target = target;
}

MaybeError OrbitalCamera::update(float delta) {
    return std::nullopt;
}
