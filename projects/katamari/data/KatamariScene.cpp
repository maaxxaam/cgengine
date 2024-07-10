#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>

#include "KatamariScene.h"
#include "Follow.h"
#include "glm/gtx/transform.hpp"
#include <glm/gtx/string_cast.hpp>
#include "src/error.h"
#include "src/objects/components/freecamera.h"
#include "src/objects/components/orbitalcamera.h"
#include "src/vk_engine.h"
#include "src/meshes/sphere.h"
#include "src/objects/components/tag.h"

tl::expected<int, Error*> KatamariScene::init(VulkanEngine* engine) {
    auto initResult = loadMeshes(engine)
    .and_then([&](int _) { return loadImages(engine); })
    .and_then([&](int _) { return initScene(engine); });

	if (!initResult.has_value()) {
		return tl::unexpected(new Error(initResult.error(), ErrorMessage("Scene initialization failed")));
	};

    return 0;
}

tl::expected<int, Error*> KatamariScene::loadMeshes(VulkanEngine* engine) {
	Mesh planeMesh{};
	//make the array 6 vertices long (2 tris)
	planeMesh._vertices.resize(6);

	//vertex positions
	planeMesh._vertices[0].position = {  1000.f, 0.f, 1000.f };
	planeMesh._vertices[1].position = { -1000.f,0.f,  1000.f };
	planeMesh._vertices[2].position = { -1000.f,0.f, -1000.f };
	planeMesh._vertices[3].position = {  1000.f, 0.f, 1000.f };
	planeMesh._vertices[4].position = { -1000.f,0.f, -1000.f };
	planeMesh._vertices[5].position = {  1000.f, 0.f,-1000.f };

	planeMesh._vertices[0].color = { 0.2f,0.2f, 0.2f }; // almost black
	planeMesh._vertices[1].color = { 0.2f,0.2f, 0.2f }; // almost black
	planeMesh._vertices[2].color = { 0.2f,0.2f, 0.2f }; // almost black
	planeMesh._vertices[3].color = { 0.2f,0.2f, 0.2f }; // almost black
	planeMesh._vertices[4].color = { 0.2f,0.2f, 0.2f }; // almost black
	planeMesh._vertices[5].color = { 0.2f,0.2f, 0.2f }; // almost black
	//we dont care about the vertex normals atm

	// Load some objects
	auto loadMesh = meshFromOBJ("../assets/monkey_smooth.obj");
	if (!loadMesh) {
		return tl::unexpected(new Error(loadMesh.error(), ErrorMessage("Failed to load mesh!")));
	}
	
	Mesh monkey = loadMesh.value();

	loadMesh = meshFromOBJ("../assets/basketball.obj");
	if (!loadMesh) {
		return tl::unexpected(new Error(loadMesh.error(), ErrorMessage("Failed to load mesh!")));
	}

	Mesh basketball = loadMesh.value();

	loadMesh = meshFromOBJ("../assets/wolf.obj");
	if (!loadMesh) {
		return tl::unexpected(new Error(loadMesh.error(), ErrorMessage("Failed to load mesh!")));
	}

	Mesh wolf = loadMesh.value();

	loadMesh = meshFromOBJ("../assets/Mailbox.obj");
	if (!loadMesh) {
		return tl::unexpected(new Error(loadMesh.error(), ErrorMessage("Failed to load mesh!")));
	}

	Mesh mailbox = loadMesh.value();

	loadMesh = meshFromOBJ("../assets/tree.obj");
	if (!loadMesh) {
		return tl::unexpected(new Error(loadMesh.error(), ErrorMessage("Failed to load mesh!")));
	}

	Mesh tree = loadMesh.value();

	loadMesh = meshFromOBJ("../assets/lighthouse.obj");
	if (!loadMesh) {
		return tl::unexpected(new Error(loadMesh.error(), ErrorMessage("Failed to load mesh!")));
	}

	Mesh lighthouse = loadMesh.value();

	loadMesh = meshFromOBJ("../assets/Skydome.obj");
	if (!loadMesh) {
		return tl::unexpected(new Error(loadMesh.error(), ErrorMessage("Failed to load mesh!")));
	}

	Mesh skydome = loadMesh.value();

	loadMesh = meshFromOBJ("../assets/semi1.obj");
	if (!loadMesh) {
		return tl::unexpected(new Error(loadMesh.error(), ErrorMessage("Failed to load mesh!")));
	}

	Mesh water = loadMesh.value();

	auto uploadResult = engine->upload_mesh(monkey)
	.and_then([&](int x) { return engine->upload_mesh(tree); })
	.and_then([&](int x) { return engine->upload_mesh(mailbox); })
	.and_then([&](int x) { return engine->upload_mesh(wolf); })
	.and_then([&](int x) { return engine->upload_mesh(basketball); })
	.and_then([&](int x) { return engine->upload_mesh(skydome); })
	.and_then([&](int x) { return engine->upload_mesh(planeMesh); })
	.and_then([&](int x) { return engine->upload_mesh(water); })
	.and_then([&](int x) { return engine->upload_mesh(lighthouse); });

	VK_UNEXPECTED_ERROR(uploadResult, "Failed to upload all used meshes")

	_meshes["tree"] = tree;
	_meshes["plane"] = planeMesh;
	_meshes["monkey"] = monkey;
	_meshes["mailbox"] = mailbox;
	_meshes["wolf"] = wolf;
	_meshes["basketball"] = basketball;
	_meshes["lighthouse"] = lighthouse;
	_meshes["skydome"] = skydome;
	_meshes["water"] = water;

	// add the destruction of mesh buffers to the deletion queue
	_onSceneDestruction.push_function([&]() {
		for (auto& item: _meshes) item.second._vertexBuffer.destroy();
	});

	return 0;
}

tl::expected<int, Error*> KatamariScene::loadImages(VulkanEngine* engine) {
	TextureAsset lighthouse;
	auto result = lighthouse.loadRGBAFile(*engine, "../assets/lighthouse.png");
	if (result.has_value())
		return tl::unexpected<Error*>(new Error(result.value(), ErrorMessage("Could not load texture asset for scene")));

	_loadedTextures["lighthouse_diffuse"] = lighthouse;

	TextureAsset basketball;
	result = basketball.loadRGBAFile(*engine, "../assets/basketball_texture.png");
	if (result.has_value())
		return tl::unexpected<Error*>(new Error(result.value(), ErrorMessage("Could not load texture asset for scene")));

	_loadedTextures["basketball_diffuse"] = basketball;

	TextureAsset mailbox;
	result = mailbox.loadRGBAFile(*engine, "../assets/MailboxDiffuseMap.png");
	if (result.has_value())
		return tl::unexpected<Error*>(new Error(result.value(), ErrorMessage("Could not load texture asset for scene")));

	_loadedTextures["mailbox_diffuse"] = mailbox;

	TextureAsset tree;
	result = tree.loadRGBAFile(*engine, "../assets/tree_256.png");
	if (result.has_value())
		return tl::unexpected<Error*>(new Error(result.value(), ErrorMessage("Could not load texture asset for scene")));

	_loadedTextures["tree_diffuse"] = tree;

	TextureAsset skydome;
	result = skydome.loadRGBAFile(*engine, "../assets/Skydome.png");
	if (result.has_value())
		return tl::unexpected<Error*>(new Error(result.value(), ErrorMessage("Could not load texture asset for scene")));

	_loadedTextures["skydome_diffuse"] = skydome;

	TextureAsset water;
	result = water.loadRGBAFile(*engine, "../assets/water.png");
	if (result.has_value())
		return tl::unexpected<Error*>(new Error(result.value(), ErrorMessage("Could not load texture asset for scene")));

	_loadedTextures["water_diffuse"] = water;

    // Make sure we unload the textures once the scene is over
	_onSceneDestruction.push_function([=]() {
		for (auto &asset: _loadedTextures) {
            asset.second.unload();
        }
	});

    // create materials based off new textures
	auto materialResult = getMaterial("texturedmesh");
	if (!materialResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get textured material (TODO: hardcoding a default would be nice)")));
	}
	Material* texturedMat = materialResult.value();

    auto matResult = _loadedTextures["lighthouse_diffuse"].createSimpleMaterial(*engine, *texturedMat);
    if (!matResult.has_value()) {
        return tl::unexpected(new Error(matResult.error(), ErrorMessage("Could not create material for lighthouse")));
    }

    _materials["lighthouse"] = matResult.value();

    matResult = _loadedTextures["basketball_diffuse"].createSimpleMaterial(*engine, *texturedMat);
    if (!matResult.has_value()) {
        return tl::unexpected(new Error(matResult.error(), ErrorMessage("Could not create material for basketball")));
    }

    _materials["basketball"] = matResult.value();

    matResult = _loadedTextures["mailbox_diffuse"].createSimpleMaterial(*engine, *texturedMat);
    if (!matResult.has_value()) {
        return tl::unexpected(new Error(matResult.error(), ErrorMessage("Could not create material for mailbox")));
    }

    _materials["mailbox"] = matResult.value();

    matResult = _loadedTextures["tree_diffuse"].createSimpleMaterial(*engine, *texturedMat);
    if (!matResult.has_value()) {
        return tl::unexpected(new Error(matResult.error(), ErrorMessage("Could not create material for tree")));
    }

    _materials["tree"] = matResult.value();

    matResult = _loadedTextures["skydome_diffuse"].createSimpleMaterial(*engine, *texturedMat);
    if (!matResult.has_value()) {
        return tl::unexpected(new Error(matResult.error(), ErrorMessage("Could not create material for skydome")));
    }

    _materials["skydome"] = matResult.value();

    // create materials based off new textures
	materialResult = getMaterial("texturedanimmesh");
	if (!materialResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get textured material (TODO: hardcoding a default would be nice)")));
	}
	texturedMat = materialResult.value();

    matResult = _loadedTextures["water_diffuse"].createSimpleMaterial(*engine, *texturedMat);
    if (!matResult.has_value()) {
        return tl::unexpected(new Error(matResult.error(), ErrorMessage("Could not create material for water")));
    }

    _materials["water"] = matResult.value();

	return 0;
}


tl::expected<int, Error*> KatamariScene::initScene(VulkanEngine* engine) {

	Object cameraObject = addEmptyObject();
	cameraObject.addComponent<TagComponent>("Camera");
	cameraObject.addComponent<TagComponent>("MAINCAM");
	_camera = cameraObject.addComponent<Camera>(
		glm::vec3(0.f, 0.f, 0.f), 
		glm::vec3(0.f, 0.f, 0.f), 
		glm::vec2(1280, 1040), 
		CameraPurpose::RenderTarget);

	auto meshResult = getMesh("plane");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get default material (TODO: hardcoding a default would be nice)")));
	}
	Mesh* planeMesh = meshResult.value();

	auto materialResult = getMaterial("defaultmesh");
	if (!materialResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get default material (TODO: hardcoding a default would be nice)")));
	}
	Material* material = materialResult.value();

	glm::mat4 transformMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(0.f, 0.f, 0.f));

    Object plane = addEmptyObject();
    watch_ptr<TransformComponent> planeTransform = plane.addComponent<TransformComponent>(transformMatrix);
    JPH::ShapeRefC planeShape = JPH::BoxShapeSettings(JPH::Vec3(1000.f, .1f, 1000.f)).Create().Get();
    plane.addComponent<RenderObject>(planeMesh, material);
	plane.addComponent<TagComponent>("GROUND");
    watch_ptr<RigidBodyComponent> backRigid = plane.addComponent<RigidBodyComponent>(planeShape, planeTransform, RigidBodyType::Static);
    backRigid->createAndAdd();

	transformMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(5.f, 1.f, 0.f));

    Object theBall = addRenderObject("basketball").value();
    watch_ptr<TransformComponent> ballTransform = theBall.addComponent<TransformComponent>(transformMatrix);

	transformMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(5.f, 0.f, 3.f));

    JPH::ShapeRefC ballShape = JPH::SphereShapeSettings(.1f).Create().Get();
	watch_ptr<CollisionPhysicsComponent> ballCollision = theBall.addComponent<CollisionPhysicsComponent>(ballShape, ballTransform);
	watch_ptr<DynamicCharacterController> ballController = theBall.addComponent<DynamicCharacterController>(ballShape, ballTransform);
	// TODO: this may fail, wrap it
	ballCollision->createAndAdd();
	ballController->createAndAdd();
    _controller.init(ballController.get(), Events::kKeyA, Events::kKeyD, Events::kKeyW, Events::kKeyS, "LEFT");
    Physics::ContactCallback ballCall = [this](const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, const JPH::ContactSettings &ioSettings) {
		if (inBody2.GetUserData() == 0) return;
        Physics::PhysicalCallbacks* callbacks1 = reinterpret_cast<Physics::PhysicalCallbacks*>(inBody1.GetUserData());
        Physics::PhysicalCallbacks* callbacks2 = reinterpret_cast<Physics::PhysicalCallbacks*>(inBody2.GetUserData());
		if (!callbacks2->caller.hasComponent<TagComponent>() && callbacks2->caller.hasComponent<RigidBodyComponent>()) {
			callbacks2->caller.getComponent<RigidBodyComponent>()->scheduleRemove();
			callbacks2->caller.addComponent<Follow>(callbacks1->caller.getComponent<TransformComponent>());
		}
	};
	ballController->setContactAddedCallback(ballCall);

    Object theLight = addRenderObject("lighthouse").value();
    theLight.addComponent<TransformComponent>(transformMatrix);

	transformMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(5.f, 0.f, -3.f));

    Object theTree = addRenderObject("tree").value();
    theTree.addComponent<TransformComponent>(transformMatrix);

	transformMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(0.f, 0.f, 0.f));

    Object theDome = addRenderObject("skydome").value();
    theDome.addComponent<TransformComponent>(transformMatrix);

	transformMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(40.f, 1.f, 0.f));
	glm::mat4 rotation = glm::mat4_cast(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1, 0, 0)));
	transformMatrix *= rotation;

    Object water = addRenderObject("water").value();
    water.addComponent<TransformComponent>(transformMatrix);

    // Attach camera to the ball
	cameraObject.addComponent<OrbitalCamera>(_camera, ballTransform);

	return 0;
}

MaybeError KatamariScene::update(float delta) {
    Scene::update(delta);
	for (auto & entity: getHierarchyOrderedObjects()) {
		Object item = getObject(entity);
		if (item.hasComponent<FreeCamera>()) {
			item.getComponent<FreeCamera>()->update(delta);
		}
		if (item.hasComponent<OrbitalCamera>()) {
			item.getComponent<OrbitalCamera>()->update(delta);
		}
		// if (item.hasComponent<Follow>()) {
			// item.getComponent<Follow>()->update(delta);
		// }
	}
    if (_cycle > 1.f) {
        _timerStorage.addTimer(2.f, [this]() {
            _spawner.spawn();
            return std::nullopt;
        });
        _cycle = 0.f;
    }
    _cycle += delta;
    _controller.update(delta);
    return std::nullopt;
}
