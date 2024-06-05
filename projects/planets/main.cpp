#include <iostream>

#include "src/vk_engine.h"
#include "scenes/PlanetScene.h"

int main(int argc, char* argv[]) {
	VulkanEngine engine;

	PlanetScene scene;
	engine.setScene(&scene);

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
