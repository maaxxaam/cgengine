#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "src/physics/physicsman.h"

#include "rigidbody.h"

RigidBodyComponent::RigidBodyComponent(const Object &self, JPH::ShapeRefC shape, watch_ptr<TransformComponent> transform, RigidBodyType type, std::optional<float> mass):
	ComponentBase(self), _created(false), _inSimulation(false) {
	_transform = transform;
	const glm::vec3 position = transform->getTranslation();
	const glm::quat rotation = transform->getOrientation();
	JPH::EMotionType motion = JPH::EMotionType::Static;
	int layer = Physics::Layers::NON_MOVING;
	switch (type) {
		case RigidBodyType::Static:
			motion = JPH::EMotionType::Static;
			layer = Physics::Layers::NON_MOVING;
			break;
		case RigidBodyType::Dynamic:
			motion = JPH::EMotionType::Dynamic;
			layer = Physics::Layers::MOVING;
			break;
		case RigidBodyType::Kinematic:
			motion = JPH::EMotionType::Kinematic;
			layer = Physics::Layers::MOVING;
			break;
	}
	_initSettings = JPH::BodyCreationSettings(
		shape, 
		JPH::Vec3(position.x, position.y, position.z),
		JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
		motion, layer);
	if (mass.has_value()) {
		_initSettings.mMassPropertiesOverride = JPH::MassProperties();
		_initSettings.mMassPropertiesOverride.mMass = mass.value();
	}
}

MaybeError RigidBodyComponent::create() {
	std::optional<Physics::NewBodyData> maybeBody = PhysicsMan.createBody(_initSettings);
	if (!maybeBody.has_value()) {
		return new Error("Couldn't create a rigid body! We probably exceeded MAX_BODIES");
	}
	_created = true;
	Physics::NewBodyData bodydata = maybeBody.value();
	_body = std::shared_ptr<JPH::Body>(bodydata.body);
	// _body->SetUserData(reinterpret_cast<JPH::uint64>(this));
	_id = bodydata.id;
	_created = true;
	return std::nullopt;
}

MaybeError RigidBodyComponent::createAndAdd() {
	std::optional<Physics::NewBodyData> maybeBody = PhysicsMan.createAndAddBody(_initSettings);
	if (!maybeBody.has_value()) {
		return new Error("Couldn't create a rigid body! We probably exceeded MAX_BODIES");
	}
	_created = true;
	Physics::NewBodyData bodydata = maybeBody.value();
	_body = std::shared_ptr<JPH::Body>(bodydata.body);
	// _body->SetUserData(reinterpret_cast<JPH::uint64>(this));
	_id = bodydata.id;
	_created = true;
	_inSimulation = true;
	return std::nullopt;
}

RigidBodyComponent::~RigidBodyComponent() {
	if (_body.use_count() == 1) {
		if (_inSimulation)
			PhysicsMan.removeBody(_id);
		PhysicsMan.destroyBody(_id);
	}
}

void RigidBodyComponent::add() {
	PhysicsMan.addBody(_id);
	_inSimulation = true;
}

void RigidBodyComponent::remove() {
	PhysicsMan.removeBody(_id);
	_inSimulation = false;
}

glm::vec3 RigidBodyComponent::GetPosition() const {
	return Vec3(_body->GetCenterOfMassPosition());
}

glm::vec3 RigidBodyComponent::GetVelocity() const {
	return Vec3(_body->GetLinearVelocity());
}

void RigidBodyComponent::setPosition(Vec3 position) {
	PhysicsMan.setPosition(_id, position);
}

void RigidBodyComponent::setVelocity(Vec3 velocity) {
	_body->SetLinearVelocity(velocity);
}

MaybeError RigidBodyComponent::update(float delta) {
	// Update transform
	_transform->setMatrix(_body->GetWorldTransform());

    return std::nullopt;
}

void RigidBodyComponent::applyCentralImpulse(Vec3 impulse) {
	_body->AddImpulse(impulse);
}

void RigidBodyComponent::setFriction(float friction) {
	if (_created) {
		_body->SetFriction(friction);
	} else {
		_initSettings.mFriction = friction;
	}
}

void RigidBodyComponent::setRestitution(float restitution) {
	if (_created) {
		_body->SetRestitution(restitution);
	} else {
		_initSettings.mRestitution = restitution;
	}
}

RigidBodyComponent RigidBodyComponent::Box(const Object &self, Vec3 halfExtents, watch_ptr<TransformComponent> transform) { 
	JPH::ShapeRefC boxShape = JPH::BoxShapeSettings(halfExtents).Create().Get();
	return RigidBodyComponent(self, boxShape, transform); 
}

RigidBodyComponent RigidBodyComponent::Cylinder(const Object &self, float halfHeight, float radius, watch_ptr<TransformComponent> transform) { 
	JPH::ShapeRefC boxShape = JPH::CylinderShapeSettings(halfHeight, radius).Create().Get();
	return RigidBodyComponent(self, boxShape, transform); 
}

RigidBodyComponent RigidBodyComponent::Capsule(const Object &self, float halfHeight, float radius, watch_ptr<TransformComponent> transform) { 
	JPH::ShapeRefC boxShape = JPH::CapsuleShapeSettings(halfHeight, radius).Create().Get();
	return RigidBodyComponent(self, boxShape, transform); 
}

RigidBodyComponent RigidBodyComponent::Sphere(const Object &self, float radius, watch_ptr<TransformComponent> transform) { 
	JPH::ShapeRefC boxShape = JPH::SphereShapeSettings(radius).Create().Get();
	return RigidBodyComponent(self, boxShape, transform);
}

