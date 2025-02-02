#include <iostream>

#include "src/vk_engine.h"
#include "scenes/PlanetScene.h"

int main(int argc, char* argv[]) {
	VulkanEngine engine;

	PlanetScene scene;
	engine.setScene(&scene);

	// Important!
	Physics::prepareJolt();

	auto init = engine.init();

	if (init.has_value()) {
		std::cerr << fmt::format("Engine initialization failed:\n{}\n", init.value()->what());
		delete init.value();
		return 1;
	}

	auto run = engine.run();

	if (run.has_value()) {
		std::cerr << fmt::format("Engine runtime error:\n{}\n", run.value()->what());
		delete run.value();
	}
	
	engine.cleanup();	

	PhysicsMan.destroy();

	return 0;
}
