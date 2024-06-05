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

MaybeError Scene::update(float delta) {
    _timerStorage.update(delta);
    return std::nullopt;
}
