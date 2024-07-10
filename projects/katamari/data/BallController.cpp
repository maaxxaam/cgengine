#include "BallController.h"
#include "src/events/key.h"
#include "Follow.h"


void BallController::init(DynamicCharacterController* controller, Events::Key left, Events::Key right, Events::Key up, Events::Key down, std::string prefix) {
    _strUp = prefix + "_UP";
    _strDown = prefix + "_DOWN";
    _strLeft = prefix + "_LEFT";
    _strRight = prefix + "_RIGHT";
    _hUp = Common::crc32(_strUp);
    _hDown = Common::crc32(_strDown);
    _hLeft = Common::crc32(_strLeft);
    _hRight = Common::crc32(_strRight);
    _controller = controller;
    _up = up;
    _down = down;
    _left = left;
    _right = right;
    initListener();
}

void BallController::initListener() {
    //
    Events::EventCallback callbackMove = [this](auto && PH1) { handleMovement(std::forward<decltype(PH1)>(PH1)); };

    addActionCallback({ _hUp, _hDown, _hLeft, _hRight }, callbackMove);
    bindCallbacks();

    EventMan.addBinding(_strUp, _up);
    EventMan.addBinding(_strDown, _down);
    EventMan.addBinding(_strRight, _right);
    EventMan.addBinding(_strLeft, _left);
}

void BallController::handleMovement(const Events::Event& event) {
    Events::KeyEvent kevent = std::get<Events::KeyEvent>(event.data);
    // fmt::println("Paddle event!");
    if (kevent.state == Events::kPress) {
        if (event.action == _hUp)
            _direction = glm::vec2(1.f, 0.f);    
        if (event.action == _hDown)
            _direction = glm::vec2(-1.f, 0.f);
        if (event.action == _hLeft)
            _direction = glm::vec2(.0f, -1.f);
        if (event.action == _hRight)
            _direction = glm::vec2(0.f, 1.f);
    } else if (kevent.state == Events::kRelease) {
        _controller->Stop();
        // _controller->setVelocity(glm::zero<glm::vec3>());
        _direction = glm::zero<glm::vec2>();
    }
}

MaybeError BallController::update(float delta) {
    _controller->Walk(_direction);
    glm::vec3 velocity = _controller->GetVelocity() * delta;
    _orient += velocity;
    glm::vec3 PYR = glm::vec3(_orient.z, _orient.x, 0.f);
    // fmt::println("P: {} {} {}", PYR.x, PYR.y, PYR.z);
    _controller->getObject().getComponent<TransformComponent>()->setEulerOrientation(PYR);
    Level* lvl = _controller->getObject().getLevel();
    for (auto &&[entity, follow]: lvl->_registry.view<Follow>().each()) {
		follow.update(delta);
	}
    // glm::vec3 impulse = glm::vec3(_direction.x, 0.f, _direction.y);
    // glm::vec3 impulsePos = impulse + _controller->GetPosition();
    // fmt::println("{} {} {}", impulse.x, impulse.y, impulse.z);
    // _controller->setVelocity(impulse);
    // _controller->applyTorque(impulsePos);
    // _controller->_body->AddAngularImpulse(JPH::Vec3(impulsePos.x, impulsePos.y, impulsePos.z));
    // fmt::println("{} {} | {} {}", _direction.x, _direction.y, _controller->GetVelocity().x, _controller->GetVelocity().y);
    return std::nullopt;
}
