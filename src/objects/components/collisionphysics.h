#pragma once

#include <memory>

#include "base.h"
#include "src/objects/object.h"
#include "transform.h"
#include "src/physics/physicsman.h"
#include "src/vector.h"
#include "src/watchptr.h"
#include "src/update/update.h"

class CollisionPhysicsComponent: public Update, public ComponentBase {
public:
    CollisionPhysicsComponent(Object &self, JPH::ShapeRefC shape, watch_ptr<TransformComponent> transform);
	~CollisionPhysicsComponent();

    static CollisionPhysicsComponent Box(Object &self, Vec3 halfExtents, watch_ptr<TransformComponent> transform);

	static CollisionPhysicsComponent Cylinder(Object &self, float halfHeight, float radius, watch_ptr<TransformComponent> transform);

	static CollisionPhysicsComponent Capsule(Object &self, float halfHeight, float radius, watch_ptr<TransformComponent> transform);

	static CollisionPhysicsComponent Sphere(Object &self, float radius, watch_ptr<TransformComponent> transform);

	MaybeError create();
	MaybeError createAndAdd();
	void add();
	void remove();

	bool isSimulated() const { return _inSimulation; };

	void setPosition(Vec3 position);

	void setTransform(const glm::vec3 &position, const glm::mat3 &rotation);
	glm::vec3 getTranslation() { return _transform->getTranslation(); };
	glm::quat getRotation() { return glm::quat(_transform->getMatrix()); }
	glm::mat4 getMatrix() { return _transform->getMatrix(); };

	MaybeError update(float delta) override;
private:
	watch_ptr<TransformComponent> _transform;
	watch_ptr<JPH::Body> _body;
	JPH::BodyID _id;
	JPH::BodyCreationSettings _initSettings;
	bool _created, _inSimulation;
	Physics::PhysicalCallbacks _callbacks;
};

// typedef std::shared_ptr<CollisionPhysicsComponent> CollisionPhysicsPtr;
