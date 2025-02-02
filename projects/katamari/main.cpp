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
