#pragma once

#include "src/events/eventlistener.h"
#include "base.h"
#include "camera.h"

class FreeCamera: public ComponentBase, public Update, public Events::EventListener {
public:
    float moveFactor, rotateFactor;

    FreeCamera(const Object &self, watch_ptr<Camera> camera);
    FreeCamera& operator=(FreeCamera &&other);

    virtual void initListener() override;

    MaybeError update(float delta) override;
private:
 	void handleMovement(const Events::Event &event);
	void handleRotation(const Events::Event &event);

    watch_ptr<Camera> _cam;
    glm::vec3 _PYR, _position;
    glm::vec2 _rotationInput;
    glm::vec3 _positionInput;

    // Event references
    std::string _moveUpStr, _moveDownStr, _moveLeftStr, _moveRightStr, _moveBackStr, _moveForwardsStr, _rotateMouseStr, _rotateLeftStr, _rotateRightStr, _rotateUpStr, _rotateDownStr;
    uint32_t _moveUpHash, _moveDownHash, _moveLeftHash, _moveRightHash, _moveBackHash, _moveForwardsHash, _rotateMouseHash, _rotateLeftHash, _rotateRightHash, _rotateUpHash, _rotateDownHash;

    bool _clearRotation, _clearDirection;
    const glm::vec3 _up {0.f, 1.f, 0.f};
};
