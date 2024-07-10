#include "scene.h"
#include "src/error.h"
#include <optional>

Scene::Scene() {
    //
};

tl::expected<int, Error*> Scene::init(VulkanEngine* engine) {
    auto initResult = loadMeshes(engine)
    .and_then([&](int _) { return loadImages(engine); })
    .and_then([&](int _) { return initScene(engine); });

	if (!initResult.has_value()) {
		return tl::unexpected(new Error(initResult.error(), ErrorMessage("Scene initialization failed")));
	};

    return 0;
}

Material* Scene::addMaterial(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name) {
	Material mat;
	mat.pipeline = pipeline;
	mat.pipelineLayout = layout;
	_materials[name] = mat;
	return &_materials[name];
}

Object Scene::getObject(entt::entity id) {
	return _level.getObject(id);
}

tl::expected<Object, Error*> Scene::addRenderObject(const std::string &mapName) {
	auto mat = getMaterial(mapName);
	auto mesh = getMesh(mapName);
	if (!mesh.has_value() | !mat.has_value()) {
		return tl::unexpected(new Error(ErrorMessage("Couldn't find material or mesh to create object")));
	}
	Mesh m = *(mesh.value());
	Object result = _level.addObject();
    result.addComponent<HierarchyComponent>();
	result.addComponent<RenderObject>(mesh.value(), mat.value());
    return result;
}

Object Scene::addEmptyObject() {
	Object result = _level.addObject();
    result.addComponent<HierarchyComponent>();
    return result;
}

std::optional<Material*> Scene::getMaterial(const std::string& name) {
	auto it = _materials.find(name);
	if (it == _materials.end()) {
		return std::nullopt;
	}
	return &(*it).second;
}

std::optional<Mesh*> Scene::getMesh(const std::string& name) {
	auto it = _meshes.find(name);
	if (it == _meshes.end()) {
		return std::nullopt;
	}
	return &(*it).second;
}

entityList Scene::getHierarchyOrderedObjects() {
	SimpleView<HierarchyComponent> hierarchies = _level._registry.view<HierarchyComponent>();
	entityList result;
	// Get root ids
	for (auto &&[entity, hier]: hierarchies.each()) {
		if (hier._parentId == entt::null) {
			result.push_back(entity);
		}
	}
	entityList queue = result;
	for (auto & entity: queue) {
		Object item = getObject(entity);
		watch_ptr<HierarchyComponent> itemh = item.getComponentDefault<HierarchyComponent>();
		entityList newChildren = itemh->allChildren();
		result.insert(result.end(), newChildren.begin(), newChildren.end());
	}
	return result;
}

MaybeError Scene::update(float delta) {
    _timerStorage.update(delta);
    return std::nullopt;
}
