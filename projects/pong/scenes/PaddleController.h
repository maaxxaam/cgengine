#pragma once

#include "src/crc32.h"
#include "src/physics/dynamiccontroller.h"
#include "src/events/eventlistener.h"
#include "src/update/update.h"

class PaddleController: public Events::EventListener, public Update {
public:
    void init(DynamicCharacterController* controller, Events::Key left, Events::Key right, Events::Key up, Events::Key down, std::string prefix);
    virtual void initListener() override;
    MaybeError update(float delta) override;
private:
    void handleMovement(const Events::Event& event);
    DynamicCharacterController* _controller;
    glm::vec2 _direction = glm::zero<glm::vec2>();
    Events::Key _up, _down, _left, _right;
    std::string _strUp, _strDown, _strLeft, _strRight;
    uint32_t _hUp, _hDown, _hLeft, _hRight;
};
