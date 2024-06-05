#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "collisionphysics.h"
#include "src/physics/physicsman.h"

CollisionPhysicsComponent::CollisionPhysicsComponent(const Object &self, JPH::ShapeRefC shape, watch_ptr<TransformComponent> transform):  
	ComponentBase(self), _created(false), _inSimulation(false) {
	_transform = transform;
	const glm::vec3 position = transform->getTranslation();
	const glm::quat rotation = transform->getOrientation();
	_initSettings = JPH::BodyCreationSettings(shape, 
		Vec3(position),
		JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
		JPH::EMotionType::Dynamic,
		Physics::Layers::MOVING);
	_initSettings.mIsSensor = true;
}

MaybeError CollisionPhysicsComponent::create() {
	std::optional<Physics::NewBodyData> maybeBody = PhysicsMan.createBody(_initSettings);
	if (!maybeBody.has_value()) {
		return new Error("Couldn't create a collision body! We probably exceeded MAX_BODIES");
	}
	_created = true;
	Physics::NewBodyData bodydata = maybeBody.value();
	_body = std::shared_ptr<JPH::Body>(bodydata.body);
	// _body->SetUserData(reinterpret_cast<JPH::uint64>(this));
	_id = bodydata.id;
	_created = true;
	return std::nullopt;
}

MaybeError CollisionPhysicsComponent::createAndAdd() {
	std::optional<Physics::NewBodyData> maybeBody = PhysicsMan.createAndAddBody(_initSettings);
	if (!maybeBody.has_value()) {
		return new Error("Couldn't create a collision body! We probably exceeded MAX_BODIES");
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

CollisionPhysicsComponent::~CollisionPhysicsComponent() {
	if (_body.use_count() == 1) {
		if (_inSimulation)
			PhysicsMan.removeBody(_id);
		PhysicsMan.destroyBody(_id);
	}
}

void CollisionPhysicsComponent::add() {
	PhysicsMan.addBody(_id);
	_inSimulation = true;
}

void CollisionPhysicsComponent::remove() {
	PhysicsMan.removeBody(_id);
	_inSimulation = false;
}

void CollisionPhysicsComponent::setTransform(const glm::vec3 &position, const glm::mat3 &rotation) {
	JPH::Mat44 transform;
	transform.SetTranslation(JPH::Vec3(position.x, position.y, position.z));

	glm::quat r = glm::toQuat(rotation);
	JPH::Quat q(r.x, r.y, r.z, r.w);
	transform.SetRotation(JPH::Mat44::sRotation(q));

	PhysicsMan.setPosition(_id, Vec3(position));
	_transform->setMatrix(transform);
}

MaybeError CollisionPhysicsComponent::update(float delta) {
	PhysicsMan.setPosition(_id, Vec3(_transform->getTranslation()));
	return std::nullopt;
}

CollisionPhysicsComponent CollisionPhysicsComponent::Box(const Object &self, Vec3 halfExtents, watch_ptr<TransformComponent> transform) { 
	JPH::ShapeRefC boxShape = JPH::BoxShapeSettings(halfExtents).Create().Get();
	return CollisionPhysicsComponent(self, boxShape, transform); 
}

CollisionPhysicsComponent CollisionPhysicsComponent::Cylinder(const Object &self, float halfHeight, float radius, watch_ptr<TransformComponent> transform) { 
	JPH::ShapeRefC boxShape = JPH::CylinderShapeSettings(halfHeight, radius).Create().Get();
	return CollisionPhysicsComponent(self, boxShape, transform); 
}

CollisionPhysicsComponent CollisionPhysicsComponent::Capsule(const Object &self, float halfHeight, float radius, watch_ptr<TransformComponent> transform) { 
	JPH::ShapeRefC boxShape = JPH::CapsuleShapeSettings(halfHeight, radius).Create().Get();
	return CollisionPhysicsComponent(self, boxShape, transform); 
}

CollisionPhysicsComponent CollisionPhysicsComponent::Sphere(const Object &self, float radius, watch_ptr<TransformComponent> transform) { 
	JPH::ShapeRefC boxShape = JPH::SphereShapeSettings(radius).Create().Get();
	return CollisionPhysicsComponent(self, boxShape, transform);
}
