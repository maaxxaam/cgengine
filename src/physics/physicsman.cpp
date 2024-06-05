#include <fmt/std.h>
#include <glm/gtx/matrix_decompose.hpp>

#include "physicsman.h"
#include "src/literals.h"

namespace Physics {

bool ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const {
    switch (inObject1) {
    case Layers::NON_MOVING:
        return inObject2 == Layers::MOVING; // Non moving only collides with moving
    case Layers::MOVING:
        return true; // Moving collides with everything
    default:
        JPH_ASSERT(false);
        return false;
    }
}

BPLayerInterfaceImpl::BPLayerInterfaceImpl() {
    // Create a mapping table from object to broad phase layer
    mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
    mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
}

uint BPLayerInterfaceImpl::GetNumBroadPhaseLayers() const {
    return BroadPhaseLayers::NUM_LAYERS;
}

JPH::BroadPhaseLayer BPLayerInterfaceImpl::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const {
    JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
    return mObjectToBroadPhase[inLayer];
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
const char * BPLayerInterfaceImpl::GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const {
    switch ((BroadPhaseLayer::Type)inLayer) {
    case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
    case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
    default:													JPH_ASSERT(false); return "INVALID";
    }
}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const {
    switch (inLayer1) {
    case Layers::NON_MOVING:
        return inLayer2 == BroadPhaseLayers::MOVING;
    case Layers::MOVING:
        return true;
    default:
        JPH_ASSERT(false);
        return false;
    }
}

// See: JPH::ContactListener
JPH::ValidateResult	MyContactListener::OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) {
    // fmt::println("Contact validate callback");
    JPH::ValidateResult result = JPH::ValidateResult::AcceptContact;
    if (inBody1.GetUserData() != 0) {
        PhysicalCallbacks* callbacks = reinterpret_cast<PhysicalCallbacks*>(inBody1.GetUserData());
        if (callbacks->onContactValidate.has_value()) {
            JPH::ValidateResult call = callbacks->onContactValidate.value()(inBody1, inBody2, inBaseOffset, inCollisionResult);
            result = call;
        }
    }
    if (inBody2.GetUserData() != 0) {
        PhysicalCallbacks* callbacks = reinterpret_cast<PhysicalCallbacks*>(inBody2.GetUserData());
        if (callbacks->onContactValidate.has_value()) {
            JPH::ValidateResult call = callbacks->onContactValidate.value()(inBody2, inBody1, inBaseOffset, inCollisionResult);
            if (result != JPH::ValidateResult::AcceptContact && result != call) {
                throw std::runtime_error("Bodies did not agree on contact validation");
            } else result = call;
        }
    }

    return result;
}

void MyContactListener::OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) {
    // fmt::println("A contact was added");
    if (inBody1.GetUserData() != 0) {
        PhysicalCallbacks* callbacks = reinterpret_cast<PhysicalCallbacks*>(inBody1.GetUserData());
        if (callbacks->onContactAdded.has_value()) {
            callbacks->onContactAdded.value()(inBody1, inBody2, inManifold, ioSettings);
        }
    }
    if (inBody2.GetUserData() != 0) {
        PhysicalCallbacks* callbacks = reinterpret_cast<PhysicalCallbacks*>(inBody2.GetUserData());
        if (callbacks->onContactAdded.has_value()) {
            callbacks->onContactAdded.value()(inBody2, inBody1, inManifold, ioSettings);
        }
    }
}

void MyContactListener::OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) {
    if (inBody1.GetUserData() != 0) {
        PhysicalCallbacks* callbacks = reinterpret_cast<PhysicalCallbacks*>(inBody1.GetUserData());
        if (callbacks->onContactPersisted.has_value()) {
            callbacks->onContactPersisted.value()(inBody1, inBody2, inManifold, ioSettings);
        }
    }
    if (inBody2.GetUserData() != 0) {
        PhysicalCallbacks* callbacks = reinterpret_cast<PhysicalCallbacks*>(inBody2.GetUserData());
        if (callbacks->onContactPersisted.has_value()) {
            callbacks->onContactPersisted.value()(inBody2, inBody1, inManifold, ioSettings);
        }
    }
    // fmt::println("One: {}, {}, {}", inBody1.GetCenterOfMassPosition().GetX(), inBody1.GetCenterOfMassPosition().GetZ(), inBody1.GetCenterOfMassPosition().GetZ());
    // fmt::println("Two: {}, {}, {}", inBody2.GetCenterOfMassPosition().GetX(), inBody2.GetCenterOfMassPosition().GetZ(), inBody2.GetCenterOfMassPosition().GetZ());
    // fmt::println("A contact was persisted");
}

void MyContactListener::OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) {
    // fmt::println("A contact was removed");
}

void MyBodyActivationListener::OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) {
    // fmt::println("A body got activated");
    if (inBodyUserData != 0) {
        PhysicalCallbacks* callbacks = reinterpret_cast<PhysicalCallbacks*>(inBodyUserData);
        if (callbacks->onActivate.has_value()) {
            callbacks->onActivate.value()(inBodyID, inBodyUserData);
        }
    }
}

void MyBodyActivationListener::OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) {
    // fmt::println("A body went to sleep");
    if (inBodyUserData != 0) {
        PhysicalCallbacks* callbacks = reinterpret_cast<PhysicalCallbacks*>(inBodyUserData);
        if (callbacks->onDeactivate.has_value()) {
            callbacks->onDeactivate.value()(inBodyID, inBodyUserData);
        }
    }
}

void prepareJolt() {
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();
}

PhysicsManager::PhysicsManager():
    _tempAllocator(32_MB),
    _threadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1) {
    _physicsSystem.Init(
        MAX_BODIES, 
        BODY_MUTEXES, 
        MAX_BODY_PAIRS, 
        MAX_CONTACT_CONSTRAINTS, 
        _broadPhaseLayers, 
        _objectVsBroadphaseLayerFilter, 
        _objectVsObjectLayerFilter);
    _physicsSystem.SetBodyActivationListener(&_activationListener);
    _physicsSystem.SetContactListener(&_contactListener);
    JPH::BodyInterface &temp = _physicsSystem.GetBodyInterface();
    _bodyInterface.reset(&temp);
}

void PhysicsManager::update(float delta) {
    int collisionSteps = 1;
    if (delta > (1.f / 60.f)) {
        collisionSteps = static_cast<int>(std::ceil(60.f * delta));
    }
    _physicsSystem.Update(delta, collisionSteps, &_tempAllocator, &_threadPool);
}

//const CastResult PhysicsManager::raycastStatic(glm::vec3& from, glm::vec3& to) {
//}

//const CastResult PhysicsManager::raycastController(btRigidBody* controllerBody, btPairCachingGhostObject* ghost, float yOffset) {
//}

//const CastResult PhysicsManager::raycastController(RigidBodyComponent* object, float yOffset) {
//}

//const CastResult PhysicsManager::shapeCastStatic(btConvexShape* castShape, glm::mat4& from, glm::mat4& to) {
//}

std::optional<NewBodyData> PhysicsManager::createBody(const JPH::BodyCreationSettings &settings) {
    JPH::Body* newBody = _bodyInterface->CreateBody(settings);
    if (newBody == nullptr) {
        return std::nullopt;
    };
    return NewBodyData{
        newBody,
        newBody->GetID(),
    };
}

std::optional<NewBodyData> PhysicsManager::createAndAddBody(const JPH::BodyCreationSettings &settings) {
    JPH::Body* newBody = _bodyInterface->CreateBody(settings);
    if (newBody == nullptr) {
        return std::nullopt;
    };
    _bodyInterface->AddBody(newBody->GetID(), JPH::EActivation::Activate);
    return NewBodyData{
        newBody,
        newBody->GetID(),
    };
}

JPH::Character& PhysicsManager::createCharacter(const JPH::CharacterSettings &settings, const CharacterAdditionalData &extra) {
    JPH::Character* result = new JPH::Character{&settings, extra.position, extra.rotation, extra.reference, &_physicsSystem};
    return *result;
}

void PhysicsManager::addBody(const JPH::BodyID &id) {
    _bodyInterface->AddBody(id, JPH::EActivation::Activate);
}

void PhysicsManager::removeBody(const JPH::BodyID &id) {
    _bodyInterface->RemoveBody(id);
}

void PhysicsManager::destroyBody(const JPH::BodyID &id) {
    _bodyInterface->DestroyBody(id);
}

void PhysicsManager::setLinearVelocity(const JPH::BodyID &id, JPH::Vec3Arg velocity) {
    _bodyInterface->SetLinearVelocity(id, velocity);
}

JPH::RVec3 PhysicsManager::getLinearVelocity(const JPH::BodyID &id) {
    return _bodyInterface->GetLinearVelocity(id);
}

JPH::RVec3 PhysicsManager::getCoMPosition(const JPH::BodyID &id) {
    return _bodyInterface->GetCenterOfMassPosition(id);
}

void PhysicsManager::setPosition(const JPH::BodyID &id, const JPH::Vec3Arg &position) {
    _bodyInterface->SetPosition(id, position, JPH::EActivation::Activate);
}

void PhysicsManager::optimizeBroadphase() {
    _physicsSystem.OptimizeBroadPhase();
}

void PhysicsManager::destroy() {
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

}
