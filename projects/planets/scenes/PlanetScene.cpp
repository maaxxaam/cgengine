#include "PlanetScene.h"
#include "Orbit.h"
#include "glm/gtx/transform.hpp"
#include <glm/gtx/string_cast.hpp>
#include "src/error.h"
#include "src/objects/components/freecamera.h"
#include "src/objects/components/orbitalcamera.h"
#include "src/vk_engine.h"
#include "src/meshes/sphere.h"
#include "src/objects/components/tag.h"

PlanetScene::PlanetScene(): _sphere(&_level, entt::null) {
	_sphere = addEmptyObject();
};

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

	Mesh greenSmallSphere = sphereFactory.create(1);
	for (int i = 0; i < greenSmallSphere._vertices.size(); i++) greenSmallSphere._vertices[i].color = { 0.f, 1.f, 0.f }; // pure blue
	for (int i = 0; i < greenSmallSphere._vertices.size(); i++) greenSmallSphere._vertices[i].position *= .5f;

	Mesh yellowBigSphere = sphereFactory.create(1);
	for (int i = 0; i < yellowBigSphere._vertices.size(); i++) yellowBigSphere._vertices[i].color = { 1.f, 1.f, 0.f }; // pure blue
	for (int i = 0; i < yellowBigSphere._vertices.size(); i++) yellowBigSphere._vertices[i].position *= 3.f;

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
	/*
	auto loadMesh = meshFromOBJ("../assets/monkey_smooth.obj");
	if (!loadMesh) {
		return tl::unexpected(new Error(loadMesh.error(), ErrorMessage("Failed to load mesh!")));
	}
	
	Mesh monkeyMesh = loadMesh.value();
	*/

	auto loadMesh = meshFromOBJ("../assets/lost_empire.obj");
	if (!loadMesh) {
		return tl::unexpected(new Error(loadMesh.error(), ErrorMessage("Failed to load mesh!")));
	}

	Mesh lostEmpire = loadMesh.value();

	auto uploadResult = engine->upload_mesh(triMesh)
	// .and_then([&](int x) { return engine->upload_mesh(monkeyMesh); })
	.and_then([&](int x) { return engine->upload_mesh(yellowBigSphere); })
	.and_then([&](int x) { return engine->upload_mesh(greenSmallSphere); })
	.and_then([&](int x) { return engine->upload_mesh(blueSphere); })
	.and_then([&](int x) { return engine->upload_mesh(lostEmpire); });

	VK_UNEXPECTED_ERROR(uploadResult, "Failed to upload all used meshes")

	_meshes["triangle"] = triMesh;
	// _meshes["monkey"] = monkeyMesh;
	_meshes["empire"] = lostEmpire;
	_meshes["sphere"] = blueSphere;
	_meshes["bigsphere"] = yellowBigSphere;
	_meshes["smallsphere"] = greenSmallSphere;

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

	Object cameraObject = addEmptyObject();
	cameraObject.addComponent<TagComponent>("Camera");
	cameraObject.addComponent<TagComponent>("MAINCAM");
	_camera = cameraObject.addComponent<Camera>(
		glm::vec3(0.f, 0.f, 0.f), 
		glm::vec3(0.f, 0.f, 0.f), 
		glm::vec2(1280, 1040), 
		CameraPurpose::RenderTarget);
	cameraObject.addComponent<FreeCamera>(_camera);

	auto meshResult = getMesh("bigsphere");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get sphere mesh")));
	}
	Mesh* bigSphereMesh = meshResult.value();

	meshResult = getMesh("smallsphere");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get sphere mesh")));
	}
	Mesh* smallSphereMesh = meshResult.value();

	meshResult = getMesh("sphere");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get sphere mesh")));
	}
	Mesh* sphereMesh = meshResult.value();

	auto materialResult = getMaterial("defaultmesh");
	if (!materialResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get default material (TODO: hardcoding a default would be nice)")));
	}
	Material* material = materialResult.value();

	glm::mat4 transformMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(5.f, 0.f, 0.f));

	watch_ptr<TransformComponent> sphereTrans = _sphere.addComponent<TransformComponent>(transformMatrix);
	_sphere.addComponent<RenderObject>(bigSphereMesh, material);

	Object original = addEmptyObject();
	watch_ptr<TransformComponent> originalTransform = original.addComponent<TransformComponent>(glm::translate(glm::mat4{1.f}, glm::vec3(10.f, 0.f,0.f)));
	original.addComponent<TagComponent>("Origin transform");
	_sphere.addComponent<Orbit>(originalTransform, glm::vec3(0.f, 1.f, 0.f), 0.f, 0.f, glm::vec3(0.5f, 0.5f, 0.f), 1.f);
	_sphere.addComponent<TagComponent>("Sun");

	Object sphere2 = addEmptyObject();
	sphere2.addComponent<RenderObject>(smallSphereMesh, material);
	watch_ptr<TransformComponent> sphere2Trans = sphere2.addComponent<TransformComponent>(transformMatrix);
	sphere2.addComponent<Orbit>(sphereTrans, glm::vec3(1.f, 1.f, 1.f), 1.f, 8.f, glm::vec3(-0.5f, 0.5f, 0.f), 2.f);
	sphere2.addComponent<TagComponent>("Planet 1");
	watch_ptr<HierarchyComponent> s2h = sphere2.getComponent<HierarchyComponent>();
	s2h->setParent(_sphere);
	// cameraObject.addComponent<OrbitalCamera>(_camera, sphere2Trans);

	Object sphere3 = addEmptyObject();
	sphere3.addComponent<RenderObject>(sphereMesh, material);
	watch_ptr<TransformComponent> sphere3Trans = sphere3.addComponent<TransformComponent>(transformMatrix);
	sphere3.addComponent<Orbit>(sphereTrans, glm::vec3(2.f, 0.5f, 1.f), 4.f, 20.f, glm::vec3(-0.75f, 0.25f, 0.f), 6000.f);
	sphere3.addComponent<TagComponent>("Planet 2");
	watch_ptr<HierarchyComponent> s3h = sphere3.getComponent<HierarchyComponent>();
	s3h->setParent(_sphere);

	Object sphere4 = addEmptyObject();
	sphere4.addComponent<RenderObject>(smallSphereMesh, material);
	watch_ptr<TransformComponent> sphere4Trans = sphere4.addComponent<TransformComponent>(transformMatrix);
	sphere4.addComponent<Orbit>(sphere3Trans, glm::vec3(0.f, 3.5f, 2.f), 3.f, 5.f, glm::vec3(0.75f, 0.25f, 0.f), 3.f);
	sphere4.addComponent<TagComponent>("Planet 3");
	watch_ptr<HierarchyComponent> s4h = sphere4.getComponent<HierarchyComponent>();
	s4h->setParent(sphere3);

	Object sphere5 = addEmptyObject();
	sphere5.addComponent<RenderObject>(bigSphereMesh, material);
	watch_ptr<TransformComponent> sphere5Trans = sphere5.addComponent<TransformComponent>(transformMatrix);
	sphere5.addComponent<Orbit>(sphereTrans, glm::vec3(0.6f, 3.5f, 1.4f), 4.f, 40.f, glm::vec3(0.25f, 0.75f, 0.f), .5f);
	sphere5.addComponent<TagComponent>("Planet 4");
	watch_ptr<HierarchyComponent> s5h = sphere3.getComponent<HierarchyComponent>();
	s5h->setParent(_sphere);

	Object sphere6 = addEmptyObject();
	sphere6.addComponent<RenderObject>(sphereMesh, material);
	watch_ptr<TransformComponent> sphere6Trans = sphere6.addComponent<TransformComponent>(transformMatrix);
	sphere6.addComponent<Orbit>(sphere5Trans, glm::vec3(1.6f, 1.5f, 0.f), 1.f, 10.f, glm::vec3(0.25f, 0.75f, 0.f), .5f);
	sphere6.addComponent<TagComponent>("Planet 5");
	watch_ptr<HierarchyComponent> s6h = sphere6.getComponent<HierarchyComponent>();
	s6h->setParent(sphere5);

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

	meshResult = getMesh("triangle");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get empire mesh")));
	}
	Mesh* triMesh = meshResult.value();

	/*
	for (int x = -20; x <= 20; x++) {
		for (int y = -20; y <= 20; y++) {

			glm::mat4 translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(x, 0, y));
			glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(0.2, 0.2, 0.2));
			//tri.transformMatrix = translation * scale;

			//_renderables.push_back(tri);
		}
	}
	*/

	/*
	materialResult = getMaterial("texturedmesh");
	if (!materialResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get textured material (TODO: hardcoding a default would be nice)")));
	}
	Material* texturedMat = materialResult.value();

    auto createResult = engine->addSingleTextureDescriptor(_loadedTextures["empire_diffuse"].getData().imageView);
    VK_UNEXPECTED_ERROR(createResult, "Could not create a texture descriptor")
    texturedMat->textureSet = createResult.value();
	*/

	return 0;
}

MaybeError PlanetScene::update(float delta) {
    Scene::update(delta);
	// Orbit* orbit = _sphere.getComponent<Orbit>();
	for (auto & entity: getHierarchyOrderedObjects()) {
		Object item = getObject(entity);
		// fmt::println("{}", item.getComponent<TagComponent>()->getName());
		if (item.hasComponent<Orbit>()) {
			watch_ptr<TransformComponent> orbitTransform = item.getComponent<TransformComponent>();
			watch_ptr<Orbit> orbit = item.getComponent<Orbit>();
			orbit->update(delta);
			orbit->applyTo(orbitTransform);
		}
		if (item.hasComponent<FreeCamera>()) {
			item.getComponent<FreeCamera>()->update(delta);
		}
		if (item.hasComponent<OrbitalCamera>()) {
			item.getComponent<OrbitalCamera>()->update(delta);
		}
	}
	watch_ptr<TransformComponent> trans = _sphere.getComponent<TransformComponent>();
	// orbit->update(delta);
	// fmt::println("Planet {}", glm::to_string(orbit->getMatrix()));
	// orbit->applyTo(trans);
    return std::nullopt;
}
