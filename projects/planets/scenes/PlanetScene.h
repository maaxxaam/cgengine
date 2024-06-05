#pragma once

#include "src/error.h"
#include "src/scene.h"
#include "Orbit.h"

class PlanetScene: public Scene {
public:
    PlanetScene();
    ~PlanetScene() {};

    virtual tl::expected<int, Error*> init(VulkanEngine *engine) override;

private:
    Camera* _camera;
    Object _sphere;

    tl::expected<int, Error*> loadMeshes(VulkanEngine* engine) override;

    tl::expected<int, Error*> loadImages(VulkanEngine* engine) override;

    tl::expected<int, Error*> initScene(VulkanEngine* engine) override;

    MaybeError update(float delta) override;
};
