#pragma once

#include <glm/glm.hpp>

#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <memory>
#include <optional>

#include "src/singleton.h"

struct CharacterAdditionalData {
	JPH::Vec3 position;
	JPH::Quat rotation;
	JPH::uint64 reference;
};

namespace Physics {

namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};

namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr uint NUM_LAYERS(2);
};

struct CastResult {
	const bool hasHit;
	const float hitFraction;
	const glm::vec3 rayHitPoint;
	const glm::vec3 exactHitPoint;
};

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override;
};

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
	BPLayerInterfaceImpl();

	virtual uint GetNumBroadPhaseLayers() const override;

	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char * GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override;
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override;
};

// An example contact listener
class MyContactListener : public JPH::ContactListener {
public:
	// See: JPH::ContactListener
	virtual JPH::ValidateResult	OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) override;

	virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override;

	virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override;

	virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override;
};

// An example activation listener
class MyBodyActivationListener : public JPH::BodyActivationListener
{
public:
	virtual void OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override;

	virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override;
};

using ValidateCallback = std::function<JPH::ValidateResult (const JPH::Body &, const JPH::Body &, JPH::RVec3Arg, const JPH::CollideShapeResult &)>;
using ContactCallback = std::function<void (const JPH::Body &, const JPH::Body &, const JPH::ContactManifold &, const JPH::ContactSettings &)>;
using ActivationCallback = std::function<void (const JPH::BodyID &, JPH::uint64)>;

struct PhysicalCallbacks {
	std::optional<ValidateCallback> onContactValidate;
	std::optional<ContactCallback> onContactAdded;
	std::optional<ContactCallback> onContactPersisted;
	std::optional<ActivationCallback> onActivate;
	std::optional<ActivationCallback> onDeactivate;
};

const uint32_t MAX_BODIES = 65535;
const uint32_t BODY_MUTEXES = 0; // Means default Jolt settings
const uint32_t MAX_BODY_PAIRS = 65535;
const uint32_t MAX_CONTACT_CONSTRAINTS = 16383;

struct NewBodyData {
	JPH::Body* body;
	JPH::BodyID id;
};

void prepareJolt();

class PhysicsManager : public Singleton<PhysicsManager> {
public:
	PhysicsManager();

	void update(float delta);

	std::optional<NewBodyData> createBody(const JPH::BodyCreationSettings &settings);
	std::optional<NewBodyData> createAndAddBody(const JPH::BodyCreationSettings &settings);
	JPH::Character& createCharacter(const JPH::CharacterSettings &settings, const CharacterAdditionalData &extra);

	void addBody(const JPH::BodyID &id);
	JPH::Body getBody(const JPH::BodyID &id);
	void removeBody(const JPH::BodyID &id);

	void destroyBody(const JPH::BodyID &id);

	// Actions on rigid bodies
	void setLinearVelocity(const JPH::BodyID &id, JPH::Vec3Arg velocity);
	JPH::RVec3 getLinearVelocity(const JPH::BodyID &id);

	JPH::RVec3 getCoMPosition(const JPH::BodyID &id);
	void setPosition(const JPH::BodyID &id, const JPH::Vec3Arg &position);
	// const CastResult raycastStatic(glm::vec3& from, glm::vec3& to);
	// const CastResult raycastController(btRigidBody* controllerBody, btPairCachingGhostObject* ghost, float yOffset);
	// const CastResult raycastController(RigidBodyComponent* object, float yOffset);
	// const CastResult shapeCastStatic(btConvexShape* castShape, glm::mat4& from, glm::mat4& to);

	void optimizeBroadphase();

	void destroy();
private:
	//
    JPH::TempAllocatorImpl _tempAllocator;
	JPH::JobSystemThreadPool _threadPool;
	JPH::PhysicsSystem _physicsSystem;
	std::unique_ptr<JPH::BodyInterface> _bodyInterface;

	// Interfaces
	BPLayerInterfaceImpl _broadPhaseLayers;
	ObjectVsBroadPhaseLayerFilterImpl _objectVsBroadphaseLayerFilter;
	ObjectLayerPairFilterImpl _objectVsObjectLayerFilter;

	// Listeners
	MyBodyActivationListener _activationListener;
	MyContactListener _contactListener;

};

} // End of Physics

template<> inline Singleton<Physics::PhysicsManager>::~Singleton() {
	instance().destroy();
}

#define PhysicsMan Physics::PhysicsManager::instance()