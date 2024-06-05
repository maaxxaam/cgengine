#pragma once

#include "cameracontroller.h"

class OrbitalCamera: public CameraController {
public:
    OrbitalCamera(glm::vec3 const* target, Camera *camera);

    MaybeError update(float delta) override;
private:
    float _targetRadius, _targetArmLength;
    const glm::vec3 * _target;
    float _currentRadius, _currentArmLength;
};
