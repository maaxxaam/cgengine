#include "glm/ext/scalar_constants.hpp"

#include "src/objects/components/tag.h"

#include "freecamera.h"

FreeCamera::FreeCamera(const Object &self, watch_ptr<Camera> camera): 
    ComponentBase(self), _cam(camera),
    moveFactor(1.f), rotateFactor(10.f),
    _positionInput(0.f), _rotationInput(0.f),
    _clearRotation(false), _clearDirection(false) {
    _PYR = _cam->getOrientation();
    _position = _cam->getPosition();

    initListener();
}

FreeCamera& FreeCamera::operator=(FreeCamera &&other) {
    _self = other._self;
    _cam = other._cam;
    moveFactor = other.moveFactor;
    rotateFactor = other.rotateFactor;
    _positionInput = other._positionInput;
    _rotationInput = other._rotationInput;
    _clearRotation = other._clearRotation;
    _clearDirection = other._clearDirection;
    _PYR = other._PYR;
    _position = other._position;

    initListener();

    return *this;
}

void FreeCamera::initListener() {
    std::string name = "FREECAM";
    if (_self.hasComponent<TagComponent>()) {
        name += "_";
        name += _self.getComponent<TagComponent>()->getName();
    }
    _moveUpStr = name + "_MOVE_UP";
    _moveUpHash = Common::crc32(_moveUpStr);
    _moveDownStr = name + "_MOVE_DOWN";
    _moveDownHash = Common::crc32(_moveDownStr);

    _moveLeftStr = name + "_MOVE_LEFT";
    _moveLeftHash = Common::crc32(_moveLeftStr);
    _moveRightStr = name + "_MOVE_RIGHT";
    _moveRightHash = Common::crc32(_moveRightStr);

    _moveForwardsStr = name + "_MOVE_FORWARDS";
    _moveForwardsHash = Common::crc32(_moveForwardsStr);
    _moveBackStr = name + "_MOVE_BACKWARDS";
    _moveBackHash = Common::crc32(_moveBackStr);

    _rotateMouseStr = name + "_ROTATE_MOUSE";
    _rotateMouseHash = Common::crc32(_rotateMouseStr);

    _rotateUpStr = name + "_ROTATE_UP";
    _rotateUpHash = Common::crc32(_rotateUpStr);
    _rotateDownStr = name + "_ROTATE_DOWN";
    _rotateDownHash = Common::crc32(_rotateDownStr);

    _rotateLeftStr = name + "_ROTATE_LEFT";
    _rotateLeftHash = Common::crc32(_rotateLeftStr);
    _rotateRightStr = name + "_ROTATE_RIGHT";
    _rotateRightHash = Common::crc32(_rotateRightStr);

	Events::EventCallback callbackMove = [this](auto && PH1) { handleMovement(std::forward<decltype(PH1)>(PH1)); };
	Events::EventCallback callbackRotate = [this](auto && PH1) { handleRotation(std::forward<decltype(PH1)>(PH1)); };

	addActionCallback(
		{_moveBackHash, _moveForwardsHash, _moveUpHash, _moveDownHash, _moveLeftHash, _moveRightHash},
		callbackMove
	);

	addActionCallback(
		{_rotateMouseHash, _rotateDownHash, _rotateUpHash, _rotateLeftHash, _rotateRightHash},
		callbackRotate
	);

    bindCallbacks();

    EventMan.addBinding(_moveUpStr, Events::kKeyR);
    EventMan.addBinding(_moveDownStr, Events::kKeyF);
    EventMan.addBinding(_moveLeftStr, Events::kKeyA);
    EventMan.addBinding(_moveRightStr, Events::kKeyD);
    EventMan.addBinding(_moveBackStr, Events::kKeyS);
    EventMan.addBinding(_moveForwardsStr, Events::kKeyW);

    EventMan.addBinding(_rotateUpStr, Events::kKeyUp);
    EventMan.addBinding(_rotateDownStr, Events::kKeyDown);
    EventMan.addBinding(_rotateLeftStr, Events::kKeyLeft);
    EventMan.addBinding(_rotateRightStr, Events::kKeyRight);

    EventMan.add2DAxisBinding(_rotateMouseStr, Events::kMousePosition);
}

void FreeCamera::handleMovement(const Events::Event &event) {
	const auto keyEvent = std::get_if<Events::KeyEvent>(&event.data);
	const auto axisEvent = std::get_if<Events::AxisEvent<glm::vec2>>(&event.data);
	if (keyEvent) {
		_clearDirection = false;
        // fmt::println("Key state: {} {}", event.action, static_cast<int>(keyEvent->state));
        if (event.action == _moveUpHash) _positionInput.y = (keyEvent->state != Events::kRelease) ?  1.0 : 0.0;
        if (event.action == _moveDownHash) _positionInput.y = (keyEvent->state != Events::kRelease) ? -1.0 : 0.0;
        if (event.action == _moveForwardsHash) _positionInput.z = (keyEvent->state != Events::kRelease) ?  1.0 : 0.0;
        if (event.action == _moveBackHash) _positionInput.z = (keyEvent->state != Events::kRelease) ? -1.0 : 0.0;
        if (event.action == _moveLeftHash) _positionInput.x = (keyEvent->state != Events::kRelease) ?  1.0 : 0.0;
        if (event.action == _moveRightHash) _positionInput.x = (keyEvent->state != Events::kRelease) ? -1.0 : 0.0;
        /*
		if (keyEvent->state == Events::kRelease) {
			switch (event.action) {
				case kIncreaseSpeed:
					setMovementFactor(getMovementFactor() * 2.0f);
					break;
				case kDecreaseSpeed:
					setMovementFactor(getMovementFactor() * 0.5f);
					break;
			}
		}
        */
    
	} /* else if (axisEvent) {
		_clearDirection = true;
		if (event.action == kMoveGamepad) {
			_movementDirection.x = axisEvent->absolute.x;
			_movementDirection.y = axisEvent->absolute.y;
		}
	}
    */
}

void FreeCamera::handleRotation(const Events::Event &event) {
	const auto keyEvent = std::get_if<Events::KeyEvent>(&event.data);
	const auto axisEvent = std::get_if<Events::AxisEvent<glm::vec2>>(&event.data);
	if (keyEvent) {
        // fmt::println("Key state: {} {}", event.action, static_cast<int>(keyEvent->state));
		_clearRotation = false;
        if (event.action == _rotateLeftHash) _rotationInput.x = (keyEvent->state != Events::kRelease) ? -5.0 : 0.0;
        if (event.action == _rotateRightHash) _rotationInput.x = (keyEvent->state != Events::kRelease) ?  5.0 : 0.0;
        if (event.action == _rotateUpHash) _rotationInput.y = (keyEvent->state != Events::kRelease) ? -5.0 : 0.0;
        if (event.action == _rotateDownHash) _rotationInput.y = (keyEvent->state != Events::kRelease) ?  5.0 : 0.0;
        /*
		if (keyEvent->state == Events::kRelease) {
			switch (event.action) {
				case kIncreaseSensitivity:
					setRotationFactor(getRotationFactor() * 2.0f);
					break;
				case kDecreaseSensitivity:
					setRotationFactor(getRotationFactor() * 0.5f);
					break;
			}
		}
        */
	} else if (axisEvent) {
		_clearRotation = true;
		if (event.action == _rotateMouseHash) {
			_rotationInput.x = axisEvent->delta.x;
			_rotationInput.y = axisEvent->delta.y;
		} /* else if (event.action == kRotateGamepad) {
			_movementRotation.x = axisEvent->absolute.x;
			_movementRotation.y = axisEvent->absolute.y;
		} */
	}
}

MaybeError FreeCamera::update(float delta) {
    constexpr double cameraPitchLimit = glm::pi<float>() / 2 - 1e-5;
    // fmt::println("Pos input: {} {} {}", _positionInput.x, _positionInput.y, _positionInput.z);
    // fmt::println("Rot input: {} {}", _rotationInput.x, _rotationInput.y);
	_PYR.x += glm::radians(_rotationInput.x * delta * rotateFactor);
	_PYR.y -= glm::radians(_rotationInput.y * delta * rotateFactor);
	// roll is not used so far
	// _PYR.z += glm::radians(_rotationInput.z * delta * rotateFactor);
	// limit pitch to -90..90 degree range
	_PYR.y = glm::clamp(double(_PYR.y), -cameraPitchLimit, cameraPitchLimit);
    // fmt::println("Camera: {} {}", _PYR.x, _PYR.y);
    _cam->setOrientation(_PYR);
    const glm::vec3 direction = _cam->getRotation();

	glm::vec3 right = glm::cross(_up, direction);

	_position += delta * moveFactor * _positionInput.x * 100.0f * right;
	_position += delta * moveFactor * _positionInput.y * 100.0f * _up;
	_position += delta * moveFactor * _positionInput.z * 100.0f * direction;
    _cam->setPosition(_position);

	if (_clearRotation) _rotationInput = glm::vec2(0.f);
	if (_clearDirection) _positionInput = glm::vec3(0.f);
    return std::nullopt;
}