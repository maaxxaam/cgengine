#pragma once

#include "src/error.h"
#include "src/scene.h"
#include "PaddleController.h"
#include "Ball.h"
#include "src/physics/physicsman.h"

class PongScene: public Scene {
public:
    PongScene();
    ~PongScene() {};

    virtual tl::expected<int, Error*> init(VulkanEngine *engine) override;

private:
    tl::expected<int, Error*> loadMeshes(VulkanEngine* engine) override;

    tl::expected<int, Error*> loadImages(VulkanEngine* engine) override;

    tl::expected<int, Error*> initScene(VulkanEngine* engine) override;

    MaybeError update(float delta) override;

    watch_ptr<Camera> _camera;
    Object _background, _paddle1, _paddle2, _ball;
    PaddleController _controller1, _controller2;
    int _score1 = 0, _score2 = 0;
    bool _waiting = false;
    Ball _ballObject;
    std::vector<Object> _walls, _deaths;
};
