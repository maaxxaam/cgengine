#pragma once

#include <entt/entt.hpp>

#include <vector>
#include <string>

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
	Material* addMaterial(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name);
	std::optional<Material*> getMaterial(const std::string& name);

	Object addEmptyObject();
	tl::expected<Object, Error*> addRenderObject(const std::string &mapName);

	std::optional<Mesh*> getMesh(const std::string& name);

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

	std::unordered_map<std::string, Material> _materials;
	std::unordered_map<std::string, Mesh> _meshes;
	std::unordered_map<std::string, TextureAsset> _loadedTextures;

	virtual tl::expected<int, Error*> loadMeshes(VulkanEngine* engine) { return 0; };

	virtual tl::expected<int, Error*> loadImages(VulkanEngine* engine) { return 0; };

    virtual tl::expected<int, Error*> initScene(VulkanEngine* engine) { return 0; };
};
