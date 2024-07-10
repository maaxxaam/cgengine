#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include <entt/entt.hpp>

#include <optional>

#include "base.h"
#include "render.h"
#include "src/physics/physicsman.h"
#include "src/update/update.h"
#include "src/vector.h"
#include "src/error.h"
#include "src/watchptr.h"
#include "transform.h"

enum class RigidBodyType {
	Static = 1,
	Dynamic = 2,
	Kinematic = 4,
};

class Object;

struct RigidBodyComponent: public Update, public ComponentBase {
public:
    RigidBodyComponent(Object &self, JPH::ShapeRefC shape, watch_ptr<TransformComponent> transform, RigidBodyType type = RigidBodyType::Dynamic, std::optional<float> mass = std::nullopt);
    ~RigidBodyComponent();

    static RigidBodyComponent Box(Object &self, Vec3 halfExtents, watch_ptr<TransformComponent> transform);

	static RigidBodyComponent Cylinder(Object &self, float halfHeight, float radius, watch_ptr<TransformComponent> transform);

	static RigidBodyComponent Capsule(Object &self, float halfHeight, float radius, watch_ptr<TransformComponent> transform);

	static RigidBodyComponent Sphere(Object &self, float radius, watch_ptr<TransformComponent> transform);

	MaybeError create();
	MaybeError createAndAdd();
	void add();
	void remove();
	void scheduleRemove();

	bool isSimulated() const { return _inSimulation; };

	MaybeError update(float delta) override;

	void applyCentralImpulse(Vec3 impulse);
	void applyTorque(Vec3 impulse);
	void applyCentralImpulseAngular(Vec3 impulse);
	glm::vec3 GetPosition() const;
	glm::vec3 GetVelocity() const;
	void setPosition(Vec3 position);
	void setVelocity(Vec3 velocity);

	// Body creation settings
	void setFriction(float friction);
	void setRestitution(float restitution);

	JPH::BodyID getID() { return _id; };

	void setContactAddedCallback(Physics::ContactCallback callback) { _callbacks.onContactAdded = callback; };
	watch_ptr<JPH::Body> _body;
	
private:
	// Physics
	watch_ptr<TransformComponent> _transform;
	JPH::BodyID _id;
	bool _created, _inSimulation;
	JPH::BodyCreationSettings _initSettings;
	Physics::PhysicalCallbacks _callbacks;
	bool _toRemove = false;
};
