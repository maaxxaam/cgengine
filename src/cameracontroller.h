#pragma once

#include "objects/components/camera.h"
#include "update/update.h"

class CameraController: public Update {
public:
    CameraController(Camera *camera): _camera(camera) {};

    MaybeError update(float delta) override { return std::nullopt; };
private:
    Camera* _camera;
};
