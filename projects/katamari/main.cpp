#include <iostream>

#include "data/KatamariScene.h"
#include "src/vk_engine.h"

int main(int argc, char* argv[]) {
	VulkanEngine engine;

	KatamariScene scene;
	engine.setScene(&scene);

	Physics::prepareJolt();

	auto init = engine.init();
	engine._start_time = std::chrono::steady_clock::now();

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

	PhysicsMan.destroy();

	return 0;
}
