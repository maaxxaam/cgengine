#pragma once

#include "base.h"
#include "src/gpustructs.h"

enum class CameraProjection {
    Perspective,
    Orthographic
};

enum class CameraPurpose {
    RenderTarget = 1,
    LightTarget = 2,
};

class Camera: public ComponentBase {
public:
    Camera(const Object &self, glm::vec3 position, glm::vec3 orientationPYR, glm::vec2 viewportSize, CameraPurpose purpose);

    void setProjection(CameraProjection type);

    const GPUCameraData& operator()() const { return _data; };

    CameraPurpose getPurpose() const { return _purpose; };

    void setPosition(glm::vec3 position);
    void setOrientation(glm::vec3 orientationPYR);

    glm::vec3 getPosition() { return _position; };
    glm::vec3 getOrientation() { return _orientationPYR; };
    glm::vec3 getRotation() { return _rotation; };
    glm::vec2 getViewport() { return _viewportSize; };
protected:
    inline void updateProjection();
    inline void updateRotation();
    inline void updateCameraData();
    glm::vec2 _viewportSize;
    glm::vec3 _position, _rotation, _orientationPYR;
    CameraProjection _projectionType;
    CameraPurpose _purpose;
    GPUCameraData _data;
};
