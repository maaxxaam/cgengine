#pragma once

#include <glm/glm.hpp>

#include "src/objects/components/base.h"
#include "physicsman.h"
#include "src/update/update.h"
#include "src/vector.h"
#include "src/objects/components/transform.h"
#include "src/watchptr.h"
#include "src/objects/components/render.h"

class DynamicCharacterController: public Update, public ComponentBase {
public:
	float _deceleration;
	float _maxSpeed;
	float _jumpImpulse;

	//DynamicCharacterController(const glm::vec3 spawnPos, float radius, float height, float mass, float stepHeight);
	DynamicCharacterController(const Object &self, JPH::ShapeRefC shape, watch_ptr<TransformComponent> transform, std::optional<float> mass = std::nullopt);
	DynamicCharacterController(const DynamicCharacterController &other);
	~DynamicCharacterController();

    static DynamicCharacterController Box(const Object &self, Vec3 halfExtents, watch_ptr<TransformComponent> transform);

	static DynamicCharacterController Cylinder(const Object &self, float halfHeight, float radius, watch_ptr<TransformComponent> transform);

	static DynamicCharacterController Capsule(const Object &self, float halfHeight, float radius, watch_ptr<TransformComponent> transform);

	static DynamicCharacterController Sphere(const Object &self, float radius, watch_ptr<TransformComponent> transform);

	void create();
	void createAndAdd();
	void add();
	void remove();

	bool isSimulated() const { return _inSimulation; };

	MaybeError update(float delta) override;

	void applyCentralImpulse(Vec3 impulse);
	glm::vec3 GetPosition() const;
	glm::vec3 GetVelocity() const;
	void setVelocity(Vec3 velocity);

	// Acceleration vector in XZ plane
	void Walk(const glm::vec2 dir);

	// Ignores y
	void Walk(const glm::vec3 dir);

	void Stop();

	void Jump();

	bool IsOnGround() const;

	void setContactAddedCallback(Physics::ContactCallback callback) { _callbacks.onContactAdded = callback; };

	bool hasRender() { return _hasRender; };
	void addRender(RenderObject* render) {
		_render = render;
		_hasRender = true;
	}
	RenderObject* getRender() { return _render; };
private:
	// Physics
	watch_ptr<TransformComponent> _transform;
	std::shared_ptr<JPH::Character> _body;
	JPH::BodyID _id;
	JPH::CharacterSettings _initSettings;
	CharacterAdditionalData _initExtraSettings;
	bool _created, _inSimulation;

	Vec3 _manualVelocity;

	float _jumpRechargeTime, _jumpRechargeTimer;

	void UpdatePosition(float delta);
	void UpdateVelocity(float delta);

	Physics::PhysicalCallbacks _callbacks;

	bool _hasRender;
	RenderObject* _render;
};