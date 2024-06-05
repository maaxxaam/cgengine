#pragma once

#include "cameracontroller.h"

class FreeCamera: public CameraController {
public:
    FreeCamera(Camera *camera): CameraController(camera) {};

    MaybeError update(float delta) override;
private:
    //
};
