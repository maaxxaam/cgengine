#include "glm/ext/scalar_constants.hpp"
#include <glm/gtx/rotate_vector.hpp>

#include "src/objects/components/tag.h"

#include "orbitalcamera.h"

OrbitalCamera::OrbitalCamera(const Object &self, watch_ptr<Camera> camera, watch_ptr<TransformComponent> origin): 
    ComponentBase(self), _cam(camera),
    moveFactor(1.f), rotateFactor(10.f),
    _rotationInput(0.f),
    _clearRotation(false) {
    _PYR = _cam->getOrientation();
    _orbitOrigin = origin;
    _origin = {
        origin->getTranslation(),
        origin->getTranslation()
    };

    _position = calcOrbitPosition();

    initListener();
}

OrbitalCamera& OrbitalCamera::operator=(OrbitalCamera &&other) {
    _self = other._self;
    _cam = other._cam;
    _orbitOrigin = other._orbitOrigin;
    moveFactor = other.moveFactor;
    rotateFactor = other.rotateFactor;
    _origin = other._origin;
    _rotationInput = other._rotationInput;
    _clearRotation = other._clearRotation;
    _PYR = other._PYR;
    _position = other._position;

    initListener();

    return *this;
}

void OrbitalCamera::initListener() {
    std::string name = "ORBITCAM";
    if (_self.hasComponent<TagComponent>()) {
        name += "_";
        name += _self.getComponent<TagComponent>()->getName();
    }

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

	Events::EventCallback callbackRotate = [this](auto && PH1) { handleRotation(std::forward<decltype(PH1)>(PH1)); };

	addActionCallback(
		{_rotateMouseHash, _rotateDownHash, _rotateUpHash, _rotateLeftHash, _rotateRightHash},
		callbackRotate
	);

    bindCallbacks();

    EventMan.addBinding(_rotateUpStr, Events::kKeyUp);
    EventMan.addBinding(_rotateDownStr, Events::kKeyDown);
    EventMan.addBinding(_rotateLeftStr, Events::kKeyLeft);
    EventMan.addBinding(_rotateRightStr, Events::kKeyRight);

    EventMan.add2DAxisBinding(_rotateMouseStr, Events::kMousePosition);
}

void OrbitalCamera::handleRotation(const Events::Event &event) {
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

glm::vec3 OrbitalCamera::calcOrbitPosition(float radius) {
	// Convert camera direction from pitch and yaw 
	// to a 3D vector
	glm::vec3 _rotationDirection = glm::normalize(glm::vec3(
		cos(_PYR.y) * cos(_PYR.x),
		sin(_PYR.y),
		sin(_PYR.x) * cos(_PYR.y)));

	// const glm::vec3 right = glm::cross(_up, _rotationDirection);

	// constexpr float cameraPitchLimit = M_PI_2 / 3 * 2;
	/* Shoulder offset gets calculated relative to camera's yaw,
	   so with higher upper angle shoulder would get wider,
	   keeping the position between player's character and the center of the screen
	   relatively consistent. */
	// const float shoulderCoef = _shoulderCoef * (1 + std::abs(_PYR.y / cameraPitchLimit));

	/* Camera's position consists of 4 main components:
	   1. Smoothed out followed object's position;
	   2. Orbit radius opposite to view direction;
	   // 3. Shoulder offset parallel to view direction;
	   // 4. Extra rise in elevation for cases when camera
	   gets close to player's character to mimic
	   original game's behavior. */
	/*return _orbitOriginCurrent + 
		(right * _shoulderCurrent * shoulderCoef - _rotationDirection) * radius - 
		_up * radius / _orbitRadiusBase * 0.75f;*/
	return _origin.current - _rotationDirection * radius;
		// - _up * radius / _orbitRadiusBase * 0.75f;
}

MaybeError OrbitalCamera::update(float delta) {
	// Define multiple frame-independent lerping coefficients
	// to smooth out various camera movements
	const float lerpCoefSmooth = 1.0f - glm::pow(0.1f, delta);
	const float lerpCoefSnappy = 1.0f - glm::pow(1e-4, delta);
	const float lerpCoefExtraSnappy = 1.0f - glm::pow(1e-6, delta);
    _origin.target = _orbitOrigin->getTranslation();
	_origin.current = glm::mix(_origin.current, _origin.target, lerpCoefSnappy);

	// Get target look direction as pitch and yaw values
	// roll (_rotationAttitude.z) is not used so far
	_PYR.x += glm::radians(_rotationInput.x * delta * rotateFactor);
	_PYR.y -= glm::radians(_rotationInput.y * delta * rotateFactor);
	// limit pitch to -60..60 degree range 
	constexpr float cameraPitchLimit = M_PI_2 / 3 * 2;
	_PYR.y = glm::clamp(_PYR.y, -cameraPitchLimit, cameraPitchLimit);

	_position = calcOrbitPosition(_orbitRadiusBase);
    _cam->setPosition(_position);
    _cam->setOrientation(_PYR);

	// When focused, our look target is overriden, but usually we look in front of the character
    // const glm::vec3 target = _orbitOrigin->getEulerOrientation();

    // glm::vec3 _direction = _cam->getOrientation();
	// Here, we use spherical interpolation to smoothly rotate
	// our camera towards desired direction
	// if (glm::distance(_direction, target) > 1e-3)
	// _direction = glm::slerp(_direction, target, lerpCoefExtraSnappy);
    
    // _cam->setOrientation(_direction);
	// Always clear rotation for mouse/gamepad input in some way.
	// We use glm::mix to leave a bit of inertia from inputs, 
	// making camera movement way smoother to the eye
    if (_clearRotation)
	    _rotationInput = glm::mix(_rotationInput, glm::zero<glm::vec2>(), lerpCoefExtraSnappy);
    return std::nullopt;
}
