// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "expected.hpp"
#include "src/timer.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>
#include <functional>
#include <optional>

#include "platform/window.h"

#include "allocstructs.h"
#include "error.h"
#include "fence.h"
#include "deletionqueue.h"
#include "gpustructs.h"
#include "frame.h"
#include "scene.h"

struct UploadContext {
	Fence _uploadFence;
	VkCommandPool _commandPool;	
	VkCommandBuffer _commandBuffer;
};

constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine {
public:
	bool _isInitialized{ false };
	int _frameNumber {0};
	float _time;
	std::chrono::steady_clock::time_point _start_time;

	VkExtent2D _windowExtent{ 1280 , 1040 };

	std::unique_ptr<Platform::Window> _window;

	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debugMessenger;
	VkPhysicalDevice _chosenGPU;
	VkDevice _device;

	VkPhysicalDeviceProperties _gpuProperties;

	Frame _frames[FRAME_OVERLAP];
	
	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;
	
	VkRenderPass _renderPass;

	VkSurfaceKHR _surface;
	VkSwapchainKHR _swapchain;
	VkFormat _swachainImageFormat;

	std::vector<VkFramebuffer> _framebuffers;
	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;	

    DeletionQueue _onEngineShutdown;
	DeletionQueue _swapchainShutdown;
	TimerStorage _timerStorage;
	
	VmaAllocator _allocator; //vma lib allocator

	//depth resources
	VkImageView _depthImageView;
	AllocatedImage _depthImage;

	GPUSceneData _sceneParameters;
	AllocatedBuffer _sceneParameterBuffer;

	//the format for the depth image
	VkFormat _depthFormat;

	VkDescriptorPool _descriptorPool;

	VkDescriptorSetLayout _globalSetLayout;
	VkDescriptorSetLayout _objectSetLayout;
	VkDescriptorSetLayout _singleTextureSetLayout;

	Scene* _scene;

	UploadContext _uploadContext;
	//initializes everything in the engine
	std::optional<Error*> init();

	//run main loop
	std::optional<Error*> run();

	//shuts down the engine
	void cleanup();

	Frame& thisFrame();

	//our draw function
	MaybeVulkanError draw_objects(VkCommandBuffer cmd);

	size_t pad_uniform_buffer_size(size_t originalSize);

	MaybeVulkanError immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

	void setScene(Scene* scene) { _scene = scene; };

	tl::expected<VkDescriptorSet, VulkanError*> addSingleTextureDescriptor(VkImageView textureView);

	tl::expected<int, VulkanError*> upload_mesh(Mesh& mesh);
private:
	//draw loop
	std::optional<Error*> draw();
	
	std::optional<Error*> handleResize();

	tl::expected<int, Error*> initWindow();

	tl::expected<int, Error*> initVulkan();

	tl::expected<int, Error*> initSwapchain();

	tl::expected<int, Error*> initDefaultRenderpass();

	tl::expected<int, Error*> initFramebuffers();

	tl::expected<int, Error*> initCommands();

	tl::expected<int, Error*> initSyncStructures();

	tl::expected<int, Error*> initPipelines();

	tl::expected<int, Error*> initScene();

	tl::expected<int, Error*> initDescriptors();

	tl::expected<int, Error*> initFrames();

	tl::expected<int, Error*> loadScene(Scene& scene);

	tl::expected<VkShaderModule, Error*> load_shader_module(const char* filePath);
};
