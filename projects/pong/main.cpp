#include <iostream>

#include "src/vk_engine.h"
#include "scenes/PongScene.h"
#include "src/physics/physicsman.h"

int main(int argc, char* argv[]) {
	VulkanEngine engine;

	PongScene scene;
	engine.setScene(&scene);

	// Important!
	Physics::prepareJolt();

	auto init = engine.init();

	if (init.has_value()) {
		std::cerr << "Engine initialization failed:\n" << init.value()->what() << "\n";
		delete init.value();
		return 1;
	}

	auto run = engine.run();

	if (run.has_value()) {
		std::cerr << "Engine runtime error:\n" << run.value()->what() << "\n";
		delete run.value();
	}
	
	engine.cleanup();	

	return 0;
}
