#pragma once

#include "src/error.h"
#include "src/scene.h"
#include "src/watchptr.h"
#include "Spawner.h"
#include "BallController.h"

class KatamariScene: public Scene {
public:
    KatamariScene(): _spawner(this) {};
    ~KatamariScene() {};

    virtual tl::expected<int, Error*> init(VulkanEngine *engine) override;

private:
    float _cycle = 0.f;
    watch_ptr<Camera> _camera;
    Spawner _spawner;
    BallController _controller;

    tl::expected<int, Error*> loadMeshes(VulkanEngine* engine) override;

    tl::expected<int, Error*> loadImages(VulkanEngine* engine) override;

    tl::expected<int, Error*> initScene(VulkanEngine* engine) override;

    MaybeError update(float delta) override;
};
