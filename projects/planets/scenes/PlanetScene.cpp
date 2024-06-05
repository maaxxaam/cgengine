#include "PlanetScene.h"
#include "Orbit.h"
#include "glm/gtx/transform.hpp"
#include <glm/gtx/string_cast.hpp>
#include "src/error.h"
#include "src/vk_engine.h"
#include "src/meshes/sphere.h"

PlanetScene::PlanetScene(): _sphere(&_level) {};

tl::expected<int, Error*> PlanetScene::init(VulkanEngine* engine) {
    auto initResult = loadMeshes(engine)
    .and_then([&](int _) { return loadImages(engine); })
    .and_then([&](int _) { return initScene(engine); });

	if (!initResult.has_value()) {
		return tl::unexpected(new Error(initResult.error(), ErrorMessage("Scene initialization failed")));
	};

    return 0;
}

tl::expected<int, Error*> PlanetScene::loadMeshes(VulkanEngine* engine) {
	SphereCreator sphereFactory;
	Mesh blueSphere = sphereFactory.create(1);
	for (int i = 0; i < blueSphere._vertices.size(); i++) blueSphere._vertices[i].color = { 0.f, 0.f, 1.f }; // pure blue

	Mesh triMesh{};
	//make the array 3 vertices long
	triMesh._vertices.resize(3);

	//vertex positions
	triMesh._vertices[0].position = { 1.f,1.f, 0.0f };
	triMesh._vertices[1].position = { -1.f,1.f, 0.0f };
	triMesh._vertices[2].position = { 0.f,-1.f, 0.0f };

	//vertex colors, all green
	triMesh._vertices[0].color = { 0.f,1.f, 0.0f }; //pure green
	triMesh._vertices[1].color = { 0.f,1.f, 0.0f }; //pure green
	triMesh._vertices[2].color = { 0.f,1.f, 0.0f }; //pure green
	//we dont care about the vertex normals

	// Load some objects
	auto loadMesh = meshFromOBJ("../assets/monkey_smooth.obj");
	if (!loadMesh) {
		return tl::unexpected(new Error(loadMesh.error(), ErrorMessage("Failed to load mesh!")));
	}
	
	Mesh monkeyMesh = loadMesh.value();

	loadMesh = meshFromOBJ("../assets/lost_empire.obj");
	if (!loadMesh) {
		return tl::unexpected(new Error(loadMesh.error(), ErrorMessage("Failed to load mesh!")));
	}

	Mesh lostEmpire = loadMesh.value();

	auto uploadResult = engine->upload_mesh(triMesh)
	.and_then([&](int x) { return engine->upload_mesh(monkeyMesh); })
	.and_then([&](int x) { return engine->upload_mesh(blueSphere); })
	.and_then([&](int x) { return engine->upload_mesh(lostEmpire); });

	VK_UNEXPECTED_ERROR(uploadResult, "Failed to upload all used meshes")

	_meshes["triangle"] = triMesh;
	_meshes["monkey"] = monkeyMesh;
	_meshes["empire"] = lostEmpire;
	_meshes["sphere"] = blueSphere;

	// add the destruction of mesh buffers to the deletion queue
	_onSceneDestruction.push_function([&]() {
		for (auto& item: _meshes) item.second._vertexBuffer.destroy();
	});

	return 0;
}

tl::expected<int, Error*> PlanetScene::loadImages(VulkanEngine* engine) {
	TextureAsset lostEmpireAsset;
	auto result = lostEmpireAsset.loadRGBAFile(*engine, "../assets/lost_empire-RGBA.png");
	if (result.has_value())
		return tl::unexpected<Error*>(new Error(result.value(), ErrorMessage("Could not load texture asset for scene")));

	_loadedTextures["empire_diffuse"] = lostEmpireAsset;

	_onSceneDestruction.push_function([=]() {
		_loadedTextures["empire_diffuse"].unload();
	});

	return 0;
}


tl::expected<int, Error*> PlanetScene::initScene(VulkanEngine* engine) {
	_timerStorage.addTimer(3.f, []() -> std::optional<Error *> {
		fmt::println("That should run 3 seconds later...");
		return std::nullopt;
	});

	Object cameraObject = addEmptyObject();
	_camera = cameraObject.addComponent<Camera>(
		glm::vec3(0.f, 0.f, 0.f), 
		glm::vec3(0.f, 0.f, 0.f), 
		glm::vec2(1280, 1040), 
		CameraPurpose::RenderTarget);

	RenderObject sphere;
	auto meshResult = getMesh("sphere");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get sphere mesh")));
	}
	sphere.mesh = meshResult.value();

	auto materialResult = getMaterial("defaultmesh");
	if (!materialResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get default material (TODO: hardcoding a default would be nice)")));
	}
	sphere.material = materialResult.value();

	glm::mat4 transformMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(5.f, 0.f, 0.f));

	TransformComponent* sphereTrans = _sphere.addComponent<TransformComponent>(transformMatrix);
	_sphere.addComponent<RenderObject>(sphere);
	// _sphere.addComponent<Orbit>(new TransformComponent(glm::translate(glm::mat4{1.f}, glm::vec3(10.f, 0.f,0.f))), glm::vec3(0.f, 1.f, 0.f), 2.f, 1.f, glm::vec3(0.5f, 0.5f, 0.f), 1.f);

	//monkey.transformMatrix = glm::mat4{ 1.0f };

	//_renderables.push_back(monkey);

	//RenderObject map;
	//meshResult = getMesh("empire");
	//if (!meshResult) {
	//	return tl::unexpected(new Error(ErrorMessage("Could not get empire mesh")));
	//}

	//map.mesh = meshResult.value();
	//materialResult = getMaterial("texturedmesh");
	//if (!materialResult) {
	//	return tl::unexpected(new Error(ErrorMessage("Could not get textured material (TODO: hardcoding a default would be nice)")));
	//}

	//map.material = materialResult.value();
	//map.transformMatrix = glm::translate(glm::vec3{ 5,-10,0 }); //glm::mat4{ 1.0f };

	//_renderables.push_back(map);

	RenderObject tri;
	meshResult = getMesh("triangle");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get empire mesh")));
	}
	tri.mesh = meshResult.value();

	materialResult = getMaterial("defaultmesh");
	if (!materialResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get default material (TODO: hardcoding a default would be nice)")));
	}
	tri.material = materialResult.value();

	for (int x = -20; x <= 20; x++) {
		for (int y = -20; y <= 20; y++) {

			glm::mat4 translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(x, 0, y));
			glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(0.2, 0.2, 0.2));
			//tri.transformMatrix = translation * scale;

			//_renderables.push_back(tri);
		}
	}

	materialResult = getMaterial("texturedmesh");
	if (!materialResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get textured material (TODO: hardcoding a default would be nice)")));
	}
	Material* texturedMat = materialResult.value();

    auto createResult = engine->addSingleTextureDescriptor(_loadedTextures["empire_diffuse"].getData().imageView);
    VK_UNEXPECTED_ERROR(createResult, "Could not create a texture descriptor")
    texturedMat->textureSet = createResult.value();

	return 0;
}

MaybeError PlanetScene::update(float delta) {
    Scene::update(delta);
	// Orbit* orbit = _sphere.getComponent<Orbit>();
	TransformComponent* trans = _sphere.getComponent<TransformComponent>();
	// orbit->update(delta);
	// fmt::println("Planet {}", glm::to_string(orbit->getMatrix()));
	// orbit->applyTo(trans);
	fmt::println("Planet trans {}", glm::to_string(trans->getTranslation()));
    return std::nullopt;
}
