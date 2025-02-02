#pragma once

#include <entt/entt.hpp>

#include "eastloverloads.h"
#include <EASTL/vector.h>
#include <EASTL/hash_map.h>
#include <EASTL/string.h>

#include "deletionqueue.h"
#include "expected.hpp"
#include "gpustructs.h"
#include "allocstructs.h"
#include "timer.h"
#include "vk_mesh.h"
#include "vk_textures.h"
#include "update/update.h"
#include "src/objects/object.h"
#include "src/objects/components/collisionphysics.h"
#include "src/objects/components/rigidbody.h"
#include "src/physics/dynamiccontroller.h"
#include "src/objects/components/transform.h"
#include "src/objects/components/hierarchy.h"
#include "src/objects/components/ssbo.h"
#include "src/objects/components/camera.h"

class VulkanEngine;

class Scene: public Update {
public:
    Scene();
    ~Scene() {};

    virtual tl::expected<int, Error*> init(VulkanEngine* engine);

	//create material and add it to the map
	Material* addMaterial(VkPipeline pipeline, VkPipelineLayout layout, const eastl::string& name);
	std::optional<Material*> getMaterial(const eastl::string& name);

	Object addEmptyObject();
	tl::expected<Object, Error*> addRenderObject(const eastl::string &mapName);

	std::optional<Mesh*> getMesh(const eastl::string& name);

	Object getObject(entt::entity id);

	auto getSimpleRenders() { return _level._registry.view<RenderObject, TransformComponent>(); };
	SimpleView<Camera> getCameras() { return _level._registry.view<Camera>(); };
	SimpleView<DynamicCharacterController> getCharacters() { return _level._registry.view<DynamicCharacterController>(); };
	SimpleView<RigidBodyComponent> getRigidBodies() { return _level._registry.view<RigidBodyComponent>(); };
	SimpleView<CollisionPhysicsComponent> getCollisions() { return _level._registry.view<CollisionPhysicsComponent>(); };
	auto getRenders() { return _level._registry.view<RenderObject, TransformComponent, SSBOIndex>(); };

	entityList getHierarchyOrderedObjects();

    virtual MaybeError update(float delta) override;

	void flush() { _onSceneDestruction.flush(); };
protected:
    DeletionQueue _onSceneDestruction;
	TimerStorage _timerStorage;

	//default array of renderable objects
	Level _level;

	eastl::hash_map<eastl::string, Material> _materials;
	eastl::hash_map<eastl::string, Mesh> _meshes;
	eastl::hash_map<eastl::string, TextureAsset> _loadedTextures;

	virtual tl::expected<int, Error*> loadMeshes(VulkanEngine* engine) { return 0; };

	virtual tl::expected<int, Error*> loadImages(VulkanEngine* engine) { return 0; };

    virtual tl::expected<int, Error*> initScene(VulkanEngine* engine) { return 0; };
};
