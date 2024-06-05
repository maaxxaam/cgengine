#include "glm/gtc/quaternion.hpp"

#include "src/error.h"
#include "src/objects/components/transform.h"
#include "src/objects/components/collisionphysics.h"
#include "src/objects/components/rigidbody.h"
#include "src/physics/dynamiccontroller.h"
#include "src/vk_engine.h"
#include "PongScene.h"

PongScene::PongScene(): 
	Scene(), _background(&_level) ,_paddle1(&_level), _paddle2(&_level), _ball(&_level) {
	//
}

tl::expected<int, Error*> PongScene::init(VulkanEngine* engine) {
    auto initResult = loadMeshes(engine)
    .and_then([&](int _) { return loadImages(engine); })
    .and_then([&](int _) { return initScene(engine); });

	if (!initResult.has_value()) {
		return tl::unexpected(new Error(initResult.error(), ErrorMessage("Scene initialization failed")));
	};

    return 0;
}

tl::expected<int, Error*> PongScene::loadMeshes(VulkanEngine* engine) {
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
	//we dont care about the vertex normals atm

	Mesh paddleMesh{};
	//make the array 6 vertices long (2 tris)
	paddleMesh._vertices.resize(6);

	//vertex positions
	paddleMesh._vertices[0].position = { 0.5f,1.f, 0.0f };
	paddleMesh._vertices[1].position = { -0.5f,1.f, 0.0f };
	paddleMesh._vertices[2].position = { -0.5f,-1.f, 0.0f };
	paddleMesh._vertices[3].position = { 0.5f,1.f, 0.0f };
	paddleMesh._vertices[4].position = { -0.5f,-1.f, 0.0f };
	paddleMesh._vertices[5].position = { 0.5f,-1.f, 0.0f };

	for (int i = 0; i < paddleMesh._vertices.size(); i++) {
		paddleMesh._vertices[i].color = { 1.f, 1.f, 1.f };
	}

	Mesh otherPaddle{};
	otherPaddle._vertices.resize(6);

	for (int i = 0; i < otherPaddle._vertices.size(); i++) {
		otherPaddle._vertices[i].position = paddleMesh._vertices[i].position;
		otherPaddle._vertices[i].color = { 1.f, 0.f, 1.f };
	}
	//we dont care about the vertex normals atm

	Mesh squareMesh{};
	//make the array 6 vertices long (2 tris)
	squareMesh._vertices.resize(6);

	//vertex positions
	squareMesh._vertices[0].position = { 1.f,1.f, 0.0f };
	squareMesh._vertices[1].position = { -1.f,1.f, 0.0f };
	squareMesh._vertices[2].position = { -1.f,-1.f, 0.0f };
	squareMesh._vertices[3].position = { 1.f,1.f, 0.0f };
	squareMesh._vertices[4].position = { -1.f,-1.f, 0.0f };
	squareMesh._vertices[5].position = { 1.f,-1.f, 0.0f };

	squareMesh._vertices[0].color = { 0.2f,0.2f, 0.2f }; //pure black
	squareMesh._vertices[1].color = { 0.2f,0.2f, 0.2f }; //pure black
	squareMesh._vertices[2].color = { 0.2f,0.2f, 0.2f }; //pure black
	squareMesh._vertices[3].color = { 0.2f,0.2f, 0.2f }; //pure black
	squareMesh._vertices[4].color = { 0.2f,0.2f, 0.2f }; //pure black
	squareMesh._vertices[5].color = { 0.2f,0.2f, 0.2f }; //pure black
	//we dont care about the vertex normals atm

	Mesh backMesh;
	backMesh._vertices.resize(6);
	for (int i = 0; i < squareMesh._vertices.size(); i++) {
		backMesh._vertices[i].position = squareMesh._vertices[i].position * 10.f;
		backMesh._vertices[i].color = {.2f, .2f, .2f};
	}

	Mesh wallMesh;
	wallMesh._vertices.resize(6);
	wallMesh._vertices = squareMesh._vertices;
	for (int i = 0; i < wallMesh._vertices.size(); i++) {
		wallMesh._vertices[i].position = squareMesh._vertices[i].position * 5.f;
		wallMesh._vertices[i].color = { 1.f, 1.f, 1.f };
	}

	Mesh ballMesh;
	ballMesh._vertices.resize(6);
	ballMesh._vertices = squareMesh._vertices;
	for (int i = 0; i < ballMesh._vertices.size(); i++) {
		ballMesh._vertices[i].position = squareMesh._vertices[i].position * .125f;
		ballMesh._vertices[i].color = { 1.f, 1.f, 1.f };
	}

	Mesh otherBallMesh;
	otherBallMesh._vertices.resize(6);
	otherBallMesh._vertices = squareMesh._vertices;
	for (int i = 0; i < otherBallMesh._vertices.size(); i++) {
		otherBallMesh._vertices[i].position = squareMesh._vertices[i].position * .125f;
		otherBallMesh._vertices[i].color = { 1.f, 0.f, 1.f };
	}

	Mesh dangerMesh;
	dangerMesh._vertices.resize(6);
	dangerMesh._vertices = squareMesh._vertices;
	for (int i = 0; i < dangerMesh._vertices.size(); i++) {
		dangerMesh._vertices[i].position = squareMesh._vertices[i].position * 5.f;
		dangerMesh._vertices[i].color = { 1.f, 0.f, 0.f };
	}

	auto uploadResult = engine->upload_mesh(triMesh)
		.and_then([&](int _){ return engine->upload_mesh(paddleMesh); })
		.and_then([&](int _){ return engine->upload_mesh(otherPaddle); })
		.and_then([&](int _){ return engine->upload_mesh(wallMesh); })
		.and_then([&](int _){ return engine->upload_mesh(dangerMesh); })
		.and_then([&](int _){ return engine->upload_mesh(ballMesh); })
		.and_then([&](int _){ return engine->upload_mesh(otherBallMesh); })
		.and_then([&](int _){ return engine->upload_mesh(backMesh); });

	VK_UNEXPECTED_ERROR(uploadResult, "Failed to upload all used meshes")

	_meshes["triangle"] = triMesh;
	_meshes["background"] = backMesh;
	_meshes["wall"] = wallMesh;
	_meshes["danger"] = dangerMesh;
	_meshes["paddle"] = paddleMesh;
	_meshes["paddleother"] = otherPaddle;
	_meshes["ball"] = ballMesh;
	_meshes["ballother"] = otherBallMesh;

	// add the destruction of mesh buffers to the deletion queue
	_onSceneDestruction.push_function([&]() {
		for (auto& item: _meshes) item.second._vertexBuffer.destroy();
	});

	return 0;
}

tl::expected<int, Error*> PongScene::loadImages(VulkanEngine* engine) {
	return 0;
}


tl::expected<int, Error*> PongScene::initScene(VulkanEngine* engine) {
	_timerStorage.addTimer(3.f, []() -> std::optional<Error *> {
		fmt::println("That should run 3 seconds later...");
		return std::nullopt;
	});

	// Init camera
	Object cameraObject = addEmptyObject();
	_camera = cameraObject.addComponent<Camera>(
		glm::vec3(0.f, 6.f, -5.f), 
		glm::vec3(glm::pi<float>() / 2, -glm::pi<float>() / 2, 0.f), 
		glm::vec2(1280, 1040), 
		CameraPurpose::RenderTarget);

	auto materialResult = getMaterial("defaultmesh");
	if (!materialResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get default material (TODO: hardcoding a default would be nice)")));
	}
	Material* material = materialResult.value();

	auto meshResult = getMesh("background");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get background mesh")));
	}
	Mesh* backMesh = meshResult.value();

	glm::mat4 translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(0, -1, -5));
	glm::mat4 rotation = glm::mat4_cast(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1, 0, 0)));
	// glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(1, 1, 1));
	glm::mat4 transformMatrix = translation * rotation;

	_background.addComponent<RenderObject>(backMesh, material);
	watch_ptr<TransformComponent> backTransform = _background.addComponent<TransformComponent>(transformMatrix);
	JPH::ShapeRefC backShape = JPH::BoxShapeSettings(JPH::Vec3(10.f, 10.f, .5f)).Create().Get();
	watch_ptr<RigidBodyComponent> backRigid = _background.addComponent<RigidBodyComponent>(backShape, backTransform, RigidBodyType::Static);
	// TODO: this may fail, wrap it
	backRigid->createAndAdd();

	meshResult = getMesh("wall");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get wall mesh")));
	}
	Mesh* wallMesh = meshResult.value();

	Object wall = addEmptyObject();

	translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(9, 0.f, -5));
	rotation = glm::mat4_cast(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1, 0, 0)));
	// scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(1, 1, 1));
	transformMatrix = translation * rotation;

	watch_ptr<TransformComponent> wallTransform = wall.addComponent<TransformComponent>(transformMatrix);
	wall.addComponent<RenderObject>(wallMesh, material);
	_walls.push_back(wall);
	JPH::ShapeRefC wallShape = JPH::BoxShapeSettings(JPH::Vec3(5.f, 5.f, 5.f)).Create().Get();
	watch_ptr<RigidBodyComponent> wallRigid = wall.addComponent<RigidBodyComponent>(wallShape, wallTransform, RigidBodyType::Static);
	// TODO: this may fail, wrap it
	wallRigid->createAndAdd();

	wall = addEmptyObject();

	translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(-9, 0.f, -5));
	rotation = glm::mat4_cast(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1, 0, 0)));
	// scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(1, 1, 1));
	transformMatrix = translation * rotation;

	wallTransform = wall.addComponent<TransformComponent>(transformMatrix);
	wall.addComponent<RenderObject>(wallMesh, material);
	_walls.push_back(wall);
	wallShape = JPH::BoxShapeSettings(JPH::Vec3(5.f, 5.f, 5.f)).Create().Get();
	wallRigid = wall.addComponent<RigidBodyComponent>(wallShape, wallTransform, RigidBodyType::Static);
	// TODO: this may fail, wrap it
	wallRigid->createAndAdd();

	meshResult = getMesh("ball");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get wall mesh")));
	}
	Mesh* ballMesh = meshResult.value();
	translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(0, 0, -5));
	rotation = glm::mat4_cast(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1, 0, 0)));
	// scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(0.25f, 0.25f, 0.25f));
	transformMatrix = translation * rotation;

	auto ballTransform = _ball.addComponent<TransformComponent>(transformMatrix);
	watch_ptr<RenderObject> ballRender = _ball.addComponent<RenderObject>(ballMesh, material);
	// btCollisionShape* ballbt = new btBoxShape(btVector3(.125f, .125f, .5f));
	// CollisionPhysicsComponent* paddle1Collision = _paddle1.addComponent<CollisionPhysicsComponent>(CollisionPhysicsComponent::Box(glm::vec3(1.f, 1.f, .2f), paddleTransform));
	JPH::ShapeRefC ballShape = JPH::BoxShapeSettings(JPH::Vec3(.125f, .125f, .5f)).Create().Get();
	watch_ptr<RigidBodyComponent> ballRigid = _ball.addComponent<RigidBodyComponent>(ballShape, ballTransform);
	// DynamicCharacterController* ballController = _ball.addComponent<DynamicCharacterController>(ballRigid, 0.f);
	_ballObject.setBall(ballRigid.get());
	// TODO: this may fail, wrap it
	ballRigid->createAndAdd();
	ballRigid->applyCentralImpulse(glm::vec3(0.0f, 0.f, -1.f));
	// ballController->Walk(glm::vec2(1.f, -0.6f));

	meshResult = getMesh("danger");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get danger mesh")));
	}
	Mesh* dangerMesh = meshResult.value();

	Object danger = addEmptyObject();

	translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(0, 0.f, -15));
	rotation = glm::mat4_cast(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1, 0, 0)));
	// scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(5, 5, 1));
	transformMatrix = translation * rotation;

	danger.addComponent<TransformComponent>(transformMatrix);
	danger.addComponent<RenderObject>(dangerMesh, material);
	_deaths.push_back(danger);

	danger = addEmptyObject();

	translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(0, 0.f, 5));
	rotation = glm::mat4_cast(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1, 0, 0)));
	// scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(5, 5, 1));
	transformMatrix = translation * rotation;

	danger.addComponent<TransformComponent>(transformMatrix);
	danger.addComponent<RenderObject>(dangerMesh, material);
	_deaths.push_back(danger);

	meshResult = getMesh("paddle");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get paddle mesh")));
	}
	Mesh* paddleMesh = meshResult.value();

	translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(0, 0, -8.5));
	rotation = glm::mat4_cast(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1, 0, 0)));
	rotation *= glm::mat4_cast(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(0, 0, 1)));
	// scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(1, 1, 1));
	transformMatrix = translation * rotation;

	watch_ptr<TransformComponent> paddleTransform = _paddle1.addComponent<TransformComponent>(transformMatrix);
	_paddle1.addComponent<RenderObject>(paddleMesh, material);
	// btCollisionShape* paddlebt = new btBoxShape(btVector3(.5f, 1.f, .5f));
	JPH::ShapeRefC paddleShape = JPH::BoxShapeSettings(JPH::Vec3(.5f, 1.f, .5f)).Create().Get();
	watch_ptr<CollisionPhysicsComponent> paddleCollision = _paddle1.addComponent<CollisionPhysicsComponent>(paddleShape, paddleTransform);
	watch_ptr<DynamicCharacterController> paddleController = _paddle1.addComponent<DynamicCharacterController>(paddleShape, paddleTransform);
	// TODO: this may fail, wrap it
	paddleCollision->createAndAdd();
	paddleController->createAndAdd();
	_controller1.init(paddleController.get(), Events::kKeyLeft, Events::kKeyRight, Events::kKeyUp, Events::kKeyDown, "LEFT");
	RenderObject br = RenderObject(_ball, ballMesh, material);
	Physics::ContactCallback paddleCall = [br, this](const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, const JPH::ContactSettings &ioSettings) {
		const JPH::BodyID ballID = _ball.getComponent<RigidBodyComponent>()->getID();
		if (ballID == inBody2.GetID())
			_ball.addComponent<RenderObject>(br.mesh, br.material);
	};
	paddleController->setContactAddedCallback(paddleCall);
	// paddleController->addRender(br);
	
	// translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(0, -0.5, -8.5));
	translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(0, 0, -1.5));
	rotation = glm::mat4_cast(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1, 0, 0)));
	// scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(1, 1, 1));
	transformMatrix = translation * rotation;
	rotation = glm::mat4_cast(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(0, 0, 1)));
	transformMatrix *= rotation;

	meshResult = getMesh("paddleother");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get paddle mesh")));
	}
	paddleMesh = meshResult.value();

	paddleTransform = _paddle2.addComponent<TransformComponent>(transformMatrix);
	_paddle2.addComponent<RenderObject>(paddleMesh, material);
	// paddlebt = new btBoxShape(btVector3(.5f, 1.f, .5f));
	paddleCollision = _paddle2.addComponent<CollisionPhysicsComponent>(paddleShape, paddleTransform);
	paddleController = _paddle2.addComponent<DynamicCharacterController>(paddleShape, paddleTransform);
	// TODO: this may fail, wrap it
	paddleCollision->createAndAdd();
	paddleController->createAndAdd();
	_controller2.init(paddleController.get(), Events::kKeyA, Events::kKeyD, Events::kKeyW, Events::kKeyS, "RIGHT");

	meshResult = getMesh("ballother");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get paddle mesh")));
	}
	ballMesh = meshResult.value();
	RenderObject otherRender = RenderObject(_ball, ballMesh, material);
	paddleCall = [otherRender, this](const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, const JPH::ContactSettings &ioSettings) {
		const JPH::BodyID ballID = _ball.getComponent<RigidBodyComponent>()->getID();
		if (ballID == inBody2.GetID())
			_ball.addComponent<RenderObject>(otherRender.mesh, otherRender.material);
	};
	paddleController->setContactAddedCallback(paddleCall);
	// paddleRigid->addRender(otherRender);

	meshResult = getMesh("triangle");
	if (!meshResult) {
		return tl::unexpected(new Error(ErrorMessage("Could not get triangle mesh")));
	}
	Mesh* triMesh = meshResult.value();

	// Spawn a bunch of triangles for testing
	//for (int x = -20; x <= 20; x++) {
	//	for (int y = -20; y <= 20; y++) {
	//		glm::mat4 translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(x, 0, y));
	//		glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(0.2, 0.2, 0.2));
	//		glm::mat4 transformMatrix = translation * scale;

	//		Object newTriangle = addEmptyObject();
	//		newTriangle.addComponent<RenderObject>(triMesh, material);
	//		newTriangle.addComponent<TransformComponent>(transformMatrix);
	//	}
	//}

	return 0;
}

MaybeError PongScene::update(float delta) {
    Scene::update(delta);
	_controller1.update(delta);
	_controller2.update(delta);
	_ballObject.update(delta);

	glm::vec3 ballTrans = _ball.getComponent<TransformComponent>()->getTranslation();
	if (std::abs(ballTrans.z + 5) > 5 && !_waiting) {
		_waiting = true;
		_score1 += ballTrans.z > -5;
		_score2 += ballTrans.z < -5;
		_ballObject.setActive(false);
		fmt::println("Score: {} | {}", _score1, _score2);
		_timerStorage.addTimer(1.f, [this]() -> MaybeError {
			_ballObject.reset();
			_ballObject.setActive(true);
			_waiting = false;
			return std::nullopt;
		});
	}

    return std::nullopt;
}
