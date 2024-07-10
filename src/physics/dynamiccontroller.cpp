#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

#include "dynamiccontroller.h"

DynamicCharacterController::DynamicCharacterController(Object &self, JPH::ShapeRefC shape, watch_ptr<TransformComponent> transform, std::optional<float> mass):
	ComponentBase(self),
	_deceleration(0.1f), _maxSpeed(5.0f), _jumpImpulse(60.0f),
	_jumpRechargeTimer(0.0f), _jumpRechargeTime(10.0f), 
	_created(false), _inSimulation(false), _callbacks({self}) {
	_transform = transform;
	const glm::vec3 position = transform->getTranslation();
	const glm::quat rotation = transform->getOrientation();
	JPH::EMotionType motion = JPH::EMotionType::Dynamic;
	int layer = Physics::Layers::MOVING;
	_initSettings = JPH::CharacterSettings();
	_initSettings.mShape = shape;
	_initSettings.mLayer = layer;
	_initExtraSettings = {
		Vec3(position),
		JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
		// reinterpret_cast<JPH::uint64>(this)
		reinterpret_cast<JPH::uint64>(&_callbacks)
	};
	fmt::println("Pos: {} {} {}", position.x, position.y, position.z);
	if (mass.has_value()) 
		_initSettings.mMass = mass.value();
}

DynamicCharacterController::DynamicCharacterController(const DynamicCharacterController &other):
	ComponentBase(other._self), _callbacks(other._callbacks) {
	_transform = other._transform;
	_body.reset(other._body.get());
	_id = other._id;
	_initSettings = other._initSettings;
	_initExtraSettings = other._initExtraSettings;
	_created = other._created;
	_inSimulation = other._inSimulation;
}

void DynamicCharacterController::create() {
	JPH::Character &temp = PhysicsMan.createCharacter(_initSettings, _initExtraSettings);
    _body.reset(&temp);
	_id = _body->GetBodyID();
	_created = true;
}

void DynamicCharacterController::createAndAdd() {
	JPH::Character &temp = PhysicsMan.createCharacter(_initSettings, _initExtraSettings);
    _body.reset(&temp);
	_id = _body->GetBodyID();
	_created = true;
	_body->AddToPhysicsSystem();
	_inSimulation = true;
}

void DynamicCharacterController::add() {
	_body->AddToPhysicsSystem();
	_inSimulation = true;
}

void DynamicCharacterController::remove() {
	_body->RemoveFromPhysicsSystem();
}

DynamicCharacterController::~DynamicCharacterController() {
	if (_inSimulation && _body.use_count() == 1)
		_body->RemoveFromPhysicsSystem();
};

void DynamicCharacterController::Walk(const glm::vec2 dir) {
	glm::vec2 velocityXZ(dir + glm::vec2(_manualVelocity->x, _manualVelocity->z));

	// Prevent from going over maximum speed
	float speedXZ = glm::length(velocityXZ);

	if (speedXZ > _maxSpeed) {
		// fmt::println("{} ?", speedXZ);
		velocityXZ *= _maxSpeed / speedXZ;
	}

	_manualVelocity->x = velocityXZ.x;
	_manualVelocity->z = velocityXZ.y;
}

void DynamicCharacterController::Walk(const glm::vec3 dir) {
	Walk(glm::vec2(dir.x, dir.z));
}

void DynamicCharacterController::Stop() {
	_manualVelocity->x = 0;
	_manualVelocity->y = 0;
	_manualVelocity->z = 0;
}

MaybeError DynamicCharacterController::update(float delta) {
	// Update transform
	_transform->setMatrix(_body->GetWorldTransform());

	_body->SetLinearVelocity(Vec3(_manualVelocity));

	// Update jump timer
	if(_jumpRechargeTimer < _jumpRechargeTime)
		_jumpRechargeTimer += delta;

	_body->PostSimulation(0.1f);
	return std::nullopt;
}

void DynamicCharacterController::Jump() {
	if (IsOnGround() && _jumpRechargeTimer >= _jumpRechargeTime) {
		_jumpRechargeTimer = 0.0f;
		applyCentralImpulse(glm::vec3(.0f, _jumpImpulse, .0f));
	}
}

glm::vec3 DynamicCharacterController::GetPosition() const {
	return Vec3(_body->GetCenterOfMassPosition());
}

glm::vec3 DynamicCharacterController::GetVelocity() const {
	return _manualVelocity;
}

void DynamicCharacterController::setVelocity(Vec3 velocity) {
	_body->SetLinearVelocity(velocity);
}

void DynamicCharacterController::applyCentralImpulseAngular(Vec3 impulse) {
	_body->AddImpulse(impulse);
}

void DynamicCharacterController::applyTorque(Vec3 impulse) {
}

void DynamicCharacterController::applyCentralImpulse(Vec3 impulse) {
	_body->AddImpulse(impulse);
}

bool DynamicCharacterController::IsOnGround() const {
	return _body->GetGroundState() == JPH::CharacterBase::EGroundState::OnGround;
}

DynamicCharacterController DynamicCharacterController::Box(Object &self, Vec3 halfExtents, watch_ptr<TransformComponent> transform) { 
	JPH::ShapeRefC boxShape = JPH::BoxShapeSettings(halfExtents).Create().Get();
	return DynamicCharacterController(self, boxShape, transform); 
}

DynamicCharacterController DynamicCharacterController::Cylinder(Object &self, float halfHeight, float radius, watch_ptr<TransformComponent> transform) { 
	JPH::ShapeRefC boxShape = JPH::CylinderShapeSettings(halfHeight, radius).Create().Get();
	return DynamicCharacterController(self, boxShape, transform); 
}

DynamicCharacterController DynamicCharacterController::Capsule(Object &self, float halfHeight, float radius, watch_ptr<TransformComponent> transform) { 
	JPH::ShapeRefC boxShape = JPH::CapsuleShapeSettings(halfHeight, radius).Create().Get();
	return DynamicCharacterController(self, boxShape, transform); 
}

DynamicCharacterController DynamicCharacterController::Sphere(Object &self, float radius, watch_ptr<TransformComponent> transform) { 
	JPH::ShapeRefC boxShape = JPH::SphereShapeSettings(radius).Create().Get();
	return DynamicCharacterController(self, boxShape, transform);
}
