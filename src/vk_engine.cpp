
#include "expected.hpp"
#include "src/crc32.h"
#include "src/error.h"
#include "src/platform/keyconversion.h"
#include "src/platform/window.h"
#include <cstdint>
#include <fmt/core.h>
#include <memory>
#include <vulkan/vulkan_core.h>

#include <glm/gtx/string_cast.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <VkBootstrap.h>

#include <iostream>
#include <fstream>
#include <chrono>

#include "platform/gamepadconversion.h"
#include "platform/gamepadman.h"
#include "events/eventman.h"

#include "devicesingleton.h"
#include "vk_initializers.h"
#include "vk_operations.h"
#include "vk_textures.h"
#include "vmalloc.h"
#include "vk_engine.h"
#include "physics/physicsman.h"

constexpr bool bUseValidationLayers = true;

void glfwOnError(int errCode, const char *message) {
	std::cerr << "GLFW Error #" << errCode << ": " << message << "\n";
}

std::optional<Error*> VulkanEngine::init() {
	auto init_window = initWindow();

	if (!init_window.has_value()) {
		return new Error(init_window.error(), ErrorMessage("Unable to create window"));
	}

	auto init_vulkan = initVulkan()
		.and_then([&](int x) { return initSwapchain(); })
		.and_then([&](int x) { return initDefaultRenderpass(); })
		.and_then([&](int x) { return initFramebuffers(); })
		.and_then([&](int x) { return initCommands(); })
		.and_then([&](int x) { return initSyncStructures(); })
		.and_then([&](int x) { return initDescriptors(); })
		.and_then([&](int x) { return initPipelines(); })
		.and_then([&](int x) { return initFrames(); });

	if (!init_vulkan.has_value()) {
		return new Error(init_vulkan.error(), ErrorMessage("Vulkan structures initialization failed"));
	}

	// Call smth from physicsman to init it before scene init
	PhysicsMan.optimizeBroadphase();

	auto init_scene = _scene->init(this);
	
	if (!init_scene.has_value()) {
		return new Error(init_scene.error(), ErrorMessage("Scene initialization failed"));
	}

	// Everything went fine
	_isInitialized = true;

	// Optimize physics' broadphase after adding a bunch of bodies in scene.init
	PhysicsMan.optimizeBroadphase();

	return std::nullopt;
}

void VulkanEngine::cleanup() {	
	if (!_isInitialized) return;
		
	// Make sure the gpu has stopped doing its things
	vkDeviceWaitIdle(DeviceRef());

	_scene->flush();
	_swapchainShutdown.flush();
	_onEngineShutdown.flush();

	vkDestroySurfaceKHR(_instance, _surface, nullptr);

	vkDestroyDevice(DeviceRef(), nullptr);
	vkb::destroy_debug_utils_messenger(_instance, _debugMessenger);
	vkDestroyInstance(_instance, nullptr);

	//glfwDestroyWindow(_window->getWindowHandle());
	//glfwTerminate();
}

std::optional<Error*> VulkanEngine::handleResize() {
	// When minimized, window can have 0 width / height. Postpone resizing in that case
	unsigned int width = 0, height = 0;
	_window->getSize(width, height);
    while (width == 0 || height == 0) {
        glfwWaitEvents();
		_window->getSize(width, height);
    }

	// Make sure the gpu has stopped doing its things
	vkDeviceWaitIdle(DeviceRef());

	_swapchainShutdown.flush();

	auto resizeResult = initSwapchain()
		.and_then([&](int x) { return initFramebuffers(); });
	
	if (!resizeResult.has_value()) {
		return new Error(resizeResult.error(), ErrorMessage("Failed to resize swapchain"));
	}

	return std::nullopt;
}

std::optional<Error*> VulkanEngine::draw() {
	// TODO: check if window is minimized and skip drawing
	
	std::optional<VulkanError*> operationResult = thisFrame()._renderFence.wait(UINT64_MAX);
	if (operationResult) {
		return new VulkanError(operationResult.value()->getCode(), operationResult.value(), ErrorMessage("Error while waiting for previous frame to finish"));
	}

	operationResult = thisFrame()._renderFence.reset();
	if (operationResult) {
		return new VulkanError(operationResult.value()->getCode(), operationResult.value(), ErrorMessage("Couldn't reset fence for previous frame"));
	}

	operationResult = vkcommand::resetCommandBuffer(thisFrame()._mainCommandBuffer, 0);
	if (operationResult) {
		return new VulkanError(operationResult.value()->getCode(), operationResult.value(), ErrorMessage("Error while resetting command buffer for previous frame"));
	}

	// request image from the swapchain
	auto acquireResult = vkcommand::acquireNextImage(_swapchain, thisFrame()._presentSemaphore);
	if (!acquireResult) {
		if (acquireResult.error()->isResizeError()) {
			auto resizeResult = handleResize();
			if (resizeResult.has_value())
				return new Error(resizeResult.value(), ErrorMessage("Could not recreate swapchain here"));
		} else
			return new VulkanError(acquireResult.error()->getCode(), acquireResult.error(), ErrorMessage("Failed to get next image"));
	}
	uint32_t swapchainImageIndex = acquireResult.value();

	// naming it cmd for shorter writing
	VkCommandBuffer cmd = thisFrame()._mainCommandBuffer;

	// begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	operationResult = vkcommand::beginCommandBuffer(cmd, cmdBeginInfo);
	if (operationResult) {
		return new VulkanError(operationResult.value()->getCode(), operationResult.value(), ErrorMessage("Error while trying to begin command buffer"));
	}

	VkClearValue clearValue;
	// can use a changing color here
	float flash = std::abs(sin(_frameNumber / 120.f));
	clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

	// Clear depth at 1
	VkClearValue depthClear;
	depthClear.depthStencil.depth = 1.f;

	// Start the main renderpass. 
	// We will use the clear color from above, and the framebuffer of the index the swapchain gave us
	VkRenderPassBeginInfo rpInfo = vkinit::renderpass_begin_info(_renderPass, _windowExtent, _framebuffers[swapchainImageIndex]);

	// connect clear values
	rpInfo.clearValueCount = 2;

	VkClearValue clearValues[] = { clearValue, depthClear };

	rpInfo.pClearValues = &clearValues[0];
	
	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	auto drawResult  = draw_objects(cmd);	
	if (drawResult) {
		new VulkanError(drawResult.value()->getCode(), drawResult.value(), ErrorMessage("Failed to draw objects"));
	}

	vkCmdEndRenderPass(cmd);
	// finalize the command buffer (we can no longer add commands, but it can now be executed)
	operationResult = vkcommand::endCommandBuffer(cmd);
	if (operationResult) {
		return new VulkanError(operationResult.value()->getCode(), operationResult.value(), ErrorMessage("Error while trying to end command buffer"));
	}

	// prepare the submission to the queue. 
	// we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	// we will signal the _renderSemaphore, to signal that rendering has finished

	VkSubmitInfo submit = vkinit::submit_info(&cmd);
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	submit.pWaitDstStageMask = &waitStage;

	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &thisFrame()._presentSemaphore;

	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &thisFrame()._renderSemaphore;

	// submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	operationResult = vkcommand::singleQueueSubmit(_graphicsQueue, submit, thisFrame()._renderFence());
	if (operationResult) {
		return new VulkanError(operationResult.value()->getCode(), operationResult.value(), ErrorMessage("Failed to submit command buffer to graphics queue"));
	}

	// prepare present
	// this will put the image we just rendered to into the visible window.
	// we want to wait on the _renderSemaphore for that, 
	// as its necessary that drawing commands have finished before the image is displayed to the user
	VkPresentInfoKHR presentInfo = vkinit::present_info();

	presentInfo.pSwapchains = &_swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &thisFrame()._renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	operationResult = vkcommand::queuePresent(_graphicsQueue, presentInfo);
	if (operationResult) {
		if (operationResult.value()->isResizeError()) {
			auto resizeResult = handleResize();
			if (resizeResult.has_value())
				return new Error(resizeResult.value(), ErrorMessage("Could not recreate swapchain here"));
		} else
			return new VulkanError(operationResult.value()->getCode(), operationResult.value(), ErrorMessage("Error while presenting graphics queue"));
	}

	_frameNumber++;

	return std::nullopt;
}

std::optional<Error*> VulkanEngine::run() {
	bool shouldClose = false;
	std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();

	//main loop
	while (!shouldClose) {
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		const float deltaSeconds = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTime).count() * 1e-6;
		_time = std::chrono::duration_cast<std::chrono::microseconds>(now - _start_time).count() * 1e-6;

		// Update physics' objects
		PhysicsMan.update(deltaSeconds);
		for (auto &&[entity, body]: _scene->getRigidBodies().each()) {
			body.update(deltaSeconds);
		}
		for (auto &&[entity, character]: _scene->getCharacters().each()) {
			character.update(deltaSeconds);
		}
		for (auto &&[entity, collision]: _scene->getCollisions().each()) {
			collision.update(deltaSeconds);
		}

		_scene->update(deltaSeconds);

		auto drawResult = draw();
		if (drawResult) {
			return new Error(drawResult.value(), ErrorMessage("Frame draw failed"));
		}

		glfwPollEvents();
		if (glfwWindowShouldClose(_window->getWindowHandle()))
			shouldClose = true;
		lastTime = now;
	}

	return std::nullopt;
}

Frame& VulkanEngine::thisFrame() {
	return _frames[_frameNumber % FRAME_OVERLAP];
}

tl::expected<int, Error*> VulkanEngine::initWindow() {
	if (!glfwInit())
		return tl::unexpected(new Error(ErrorMessage("Failed to init GLFW. No window today :-(")));

	glfwSetErrorCallback(glfwOnError);

	if (!glfwVulkanSupported())
		return tl::unexpected(new Error(ErrorMessage("Vulkan not supported :-(")));

	_window = std::make_unique<Platform::Window>();

	// Allow locking and unlocking the mouse
	constexpr uint32_t lock = Common::crc32("Lock");
	EventMan.setActionCallback({ lock }, [&](Events::Event event){
		Events::KeyEvent key = std::get<Events::KeyEvent>(event.data);
		if(key.state == Events::kPress) {
			_window->setMouseCursorVisible(!_window->isMouseCursorVisible());}});
	EventMan.addBinding("Lock", Events::kKeyL/*, Events::kModifierAlt*/);

	GamepadMan.setGamepadButtonCallback([&](int button, int action){
		EventMan.injectGamepadButtonInput(static_cast<Events::GamepadButton>(button), action == GLFW_RELEASE? Events::kRelease : Events::kPress);
	});

	GamepadMan.setGamepadStickCallback([&](int stick, glm::vec2 pos, glm::vec2 delta){
		auto conversionResult = Platform::convertGLFW2GamepadStick(stick);
		if (conversionResult.has_value())
			EventMan.injectGamepad2DAxisInput(conversionResult.value(), pos, delta);
	});

	GamepadMan.setGamepadTriggerCallback([&](int trigger, double pos, double delta){
		auto conversionResult = Platform::convertGLFW2GamepadTrigger(trigger);
		if (conversionResult.has_value())
			EventMan.injectGamepad1DAxisInput(conversionResult.value(), pos, delta);
	});


	return 0;
}

tl::expected<int, Error*> VulkanEngine::initVulkan() {
	vkb::InstanceBuilder builder;

	//make the vulkan instance, with basic debug features
	auto inst_ret = builder.set_app_name("Example Vulkan Application")
		.request_validation_layers(bUseValidationLayers)
		.use_default_debug_messenger()
		.require_api_version(1, 1, 0)
		.build();
	if (!inst_ret.has_value()) {
		return tl::unexpected(new VulkanError(inst_ret.vk_result(), ErrorMessage("Failed to build a VulkanInstance, error: {}", inst_ret.error().message())));
	}
	vkb::Instance vkb_inst = inst_ret.value();

	_instance = vkb_inst.instance;
	_debugMessenger = vkb_inst.debug_messenger;

	auto surfaceResult = _window->createVulkanSurface(_instance, &_surface);
	VK_UNEXPECTED_ERROR(surfaceResult, "Failed to create present surface")

	// Use vkbootstrap to select a gpu. 
	// We want a gpu that can write to the GLFW surface and supports vulkan 1.2
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	auto physicalDeviceResult = selector
		.set_minimum_version(1, 1)
		.set_surface(_surface)
		.select();
	if (!physicalDeviceResult.has_value()) {
		return tl::unexpected(new Error(ErrorMessage(physicalDeviceResult.error().message())));
	}
	vkb::PhysicalDevice physicalDevice = physicalDeviceResult.value();

	//create the final vulkan device

	vkb::DeviceBuilder deviceBuilder{ physicalDevice };
	// Required for one of the shaders to work since it needs OpenGL 4.6
	// TODO: lower shader version requirements
	VkPhysicalDeviceShaderDrawParametersFeatures str{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES,
		.shaderDrawParameters = VK_TRUE
	};
	deviceBuilder.add_pNext(&str);

	auto vkbDeviceResult = deviceBuilder.build();
	if (!vkbDeviceResult.has_value()) {
		return tl::unexpected(new Error(ErrorMessage(vkbDeviceResult.error().message())));
	}
	vkb::Device vkbDevice = vkbDeviceResult.value();

	// Get the VkDevice handle used in the rest of a vulkan application
	// _device = vkbDevice.device;
	DeviceSingleton::instance().setRef(vkbDevice.device);
	_chosenGPU = physicalDevice.physical_device;

	// Use vkbootstrap to get a Graphics queue
	auto graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics);
	auto graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics);

	if (!graphicsQueue.has_value() || !graphicsQueueFamily.has_value()) {
		return tl::unexpected(new VulkanError(graphicsQueue.vk_result(), ErrorMessage("Could not find graphics queue: error {}", graphicsQueue.error().message())));
	}

	_graphicsQueue = graphicsQueue.value();
	_graphicsQueueFamily = graphicsQueueFamily.value()	;

	// Initialize the memory allocator
	VmaAllocatorCreateInfo allocatorInfo = {
		.physicalDevice = _chosenGPU,
		.device = DeviceRef(),
		.instance = _instance
	};

	VMAlloc.create(allocatorInfo);

	_onEngineShutdown.push_function([&]() {
		VMAlloc.destroy();
	});

	vkGetPhysicalDeviceProperties(_chosenGPU, &_gpuProperties);

	return 0;
}

tl::expected<int, Error*> VulkanEngine::initSwapchain() {
	vkb::SwapchainBuilder swapchainBuilder{_chosenGPU,DeviceRef(),_surface };

	auto swapchainResult = swapchainBuilder
		.use_default_format_selection()
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(_windowExtent.width, _windowExtent.height)
		.build();
	if (!swapchainResult.has_value()) {
		return tl::unexpected(new Error(ErrorMessage(swapchainResult.error().message())));
	}
	vkb::Swapchain vkbSwapchain = swapchainResult.value();

	//store swapchain and its related images
	_swapchain = vkbSwapchain.swapchain;

	auto swapchainImagesResult = vkbSwapchain.get_images();
	if (!swapchainImagesResult.has_value()) {
		return tl::unexpected(new Error(ErrorMessage(swapchainImagesResult.error().message())));
	}
	_swapchainImages = swapchainImagesResult.value();

	auto swapchainImageViewsResult = vkbSwapchain.get_image_views();
	if (!swapchainImageViewsResult.has_value()) {
		return tl::unexpected(new Error(ErrorMessage(swapchainImageViewsResult.error().message())));
	}
	_swapchainImageViews = swapchainImageViewsResult.value();

	_swachainImageFormat = vkbSwapchain.image_format;

	_swapchainShutdown.push_function([=]() {
		vkDestroySwapchainKHR(DeviceRef(), _swapchain, nullptr);
	});

	//depth image size will match the window
	VkExtent3D depthImageExtent = {
		_windowExtent.width,
		_windowExtent.height,
		1
	};

	//hardcoding the depth format to 32 bit float
	_depthFormat = VK_FORMAT_D32_SFLOAT;

	//the depth image will be a image with the format we selected and Depth Attachment usage flag
	VkImageCreateInfo dimg_info = vkinit::createinfo::image(_depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

	//for the depth image, we want to allocate it from gpu local memory
	auto depthResult = VMAlloc.createImage(VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), VMA_MEMORY_USAGE_GPU_ONLY, dimg_info);
	VK_UNEXPECTED_ERROR(depthResult, "Unable to create depth image");
	_depthImage = depthResult.value();

	//build a image-view for the depth image to use for rendering
	VkImageViewCreateInfo dview_info = vkinit::createinfo::imageView(_depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);

	auto viewResult = vkcommand::createImageView(dview_info);
	VK_UNEXPECTED_ERROR(viewResult, "Failed to create depth image view");
	_depthImageView = viewResult.value();

	//add to deletion queues
	_swapchainShutdown.push_function([=]() {
		vkDestroyImageView(DeviceRef(), _depthImageView, nullptr);
		_depthImage.destroy();
	});

	return 0;
}

tl::expected<int, Error*> VulkanEngine::initDefaultRenderpass() {
	// we define an attachment description for our main color image
	// the attachment is loaded as "clear" when renderpass start
	// the attachment is stored when renderpass ends
	// the attachment layout starts as "undefined", and transitions to "Present" so its possible to display it
	// we dont care about stencil, and dont use multisampling

	VkAttachmentDescription color_attachment = {
		.format = _swachainImageFormat,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference color_attachment_ref = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentDescription depth_attachment = {
		.flags = 0,
		.format = _depthFormat,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference depth_attachment_ref = {
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	//we are going to create 1 subpass, which is the minimum you can do
	VkSubpassDescription subpass = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment_ref,
		.pDepthStencilAttachment = &depth_attachment_ref
	};

	// dependency, which is from "outside" into the subpass. And we can read or write color
	VkSubpassDependency dependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	};

	//dependency from outside to the subpass, making this subpass dependent on the previous renderpasses
	VkSubpassDependency depth_dependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
	};

	//array of 2 dependencies, one for color, two for depth
	VkSubpassDependency dependencies[2] = { dependency, depth_dependency };

	//array of 2 attachments, one for the color, and other for depth
	VkAttachmentDescription attachments[2] = { color_attachment,depth_attachment };

	VkRenderPassCreateInfo render_pass_info = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 2,
		.pAttachments = &attachments[0],
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 2,
		.pDependencies = &dependencies[0]
	};
	
	auto passResult = vkcommand::createRenderPass(render_pass_info);
	VK_UNEXPECTED_ERROR(passResult, "Failed to create default render pass");
	_renderPass = passResult.value();

	_onEngineShutdown.push_function([=]() {
		vkDestroyRenderPass(DeviceRef(), _renderPass, nullptr);
	});

	return 0;
}

tl::expected<int, Error*> VulkanEngine::initFramebuffers() {
	//create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
	VkFramebufferCreateInfo fb_info = vkinit::createinfo::framebuffer(_renderPass, _windowExtent);

	const uint32_t swapchain_imagecount = _swapchainImages.size();
	_framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

	for (int i = 0; i < swapchain_imagecount; i++) {

		VkImageView attachments[2];
		attachments[0] = _swapchainImageViews[i];
		attachments[1] = _depthImageView;

		fb_info.pAttachments = attachments;
		fb_info.attachmentCount = 2;
		auto framebufferResult = vkcommand::createFramebuffer(fb_info);
		VK_UNEXPECTED_ERROR(framebufferResult, "Failed to create framebuffer for a swapchain image {}", i);
		_framebuffers[i] = framebufferResult.value();

		_swapchainShutdown.push_function([=]() {
			vkDestroyFramebuffer(DeviceRef(), _framebuffers[i], nullptr);
			vkDestroyImageView(DeviceRef(), _swapchainImageViews[i], nullptr);
		});
	}

	return 0;
}

tl::expected<int, Error*> VulkanEngine::initCommands() {
	// Create a command pool for commands submitted to the graphics queue.
	// We also want the pool to allow for resetting of individual command buffers
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::createinfo::commandPool(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	tl::expected<VkCommandPool, VulkanError*> poolResult;
	tl::expected<VkCommandBuffer, VulkanError*> bufferResult;

	VkCommandPoolCreateInfo uploadCommandPoolInfo = vkinit::createinfo::commandPool(_graphicsQueueFamily);
	poolResult = vkcommand::createCommandPool(commandPoolInfo);
	VK_UNEXPECTED_ERROR(poolResult, "Failed to create command buffer for upload context");
	_uploadContext._commandPool = poolResult.value();

	_onEngineShutdown.push_function([=]() {
		vkDestroyCommandPool(DeviceRef(), _uploadContext._commandPool, nullptr);
	});

	// Allocate the default command buffer that we will use for rendering
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_uploadContext._commandPool, 1);

	bufferResult = vkcommand::allocateCommandBuffer(cmdAllocInfo);
	VK_UNEXPECTED_ERROR(bufferResult, "Failed to allocate command buffer upload context");
	_uploadContext._commandBuffer = bufferResult.value();
	return 0;
}

tl::expected<int, Error*> VulkanEngine::initSyncStructures() {
	// One fence to control when the gpu has finished rendering the frame,
	// And 2 semaphores to syncronize rendering with swapchain
	// We want the fence to start signalled so we can wait on it on the first frame
	VkFenceCreateInfo fenceCreateInfo = vkinit::createinfo::fence(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::createinfo::semaphore();
	tl::expected<VkFence, VulkanError*> fenceResult;

	VkFenceCreateInfo uploadFenceCreateInfo = vkinit::createinfo::fence();

	fenceResult = vkcommand::createFence(uploadFenceCreateInfo);
	VK_UNEXPECTED_ERROR(fenceResult, "Failed to create fence for upload context");
	_uploadContext._uploadFence = {fenceResult.value()};

	_onEngineShutdown.push_function([=]() {
		_uploadContext._uploadFence.destroy();
	});

	return 0;
}

tl::expected<int, Error*> VulkanEngine::initPipelines() {
	VkShaderModule colorMeshShader;
	auto shaderResult = load_shader_module("../shaders/bin/default_lit.frag.spv");
	if (!shaderResult) {
		return tl::unexpected(new Error(shaderResult.error(), ErrorMessage("Error when building the colored mesh shader")));
	}
	colorMeshShader = shaderResult.value();

	VkShaderModule texturedMeshShader;
	shaderResult = load_shader_module("../shaders/bin/textured_lit.frag.spv");
	if (!shaderResult) {
		return tl::unexpected(new Error(shaderResult.error(), ErrorMessage("Error when building the colored mesh shader")));
	}
	texturedMeshShader = shaderResult.value();

	VkShaderModule meshVertAnimShader;
	shaderResult = load_shader_module("../shaders/bin/tri_mesh_ssbo_vertex.vert.spv");
	if (!shaderResult) {
		return tl::unexpected(new Error(shaderResult.error(), ErrorMessage("Error when building the mesh vertex shader module")));
	}
	meshVertAnimShader = shaderResult.value();

	VkShaderModule meshVertShader;
	shaderResult = load_shader_module("../shaders/bin/tri_mesh_ssbo.vert.spv");
	if (!shaderResult) {
		return tl::unexpected(new Error(shaderResult.error(), ErrorMessage("Error when building the mesh vertex shader module")));
	}
	meshVertShader = shaderResult.value();

	//build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage
	PipelineBuilder pipelineBuilder;

	std::vector<VkPushConstantRange> pushConstants { {
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.offset = 0,
		.size = sizeof(MeshPushConstants)
	} };

	pipelineBuilder._viewport = {
		0.0f,
		0.0f,
		(float)_windowExtent.width,
		(float)_windowExtent.height,
		0.0f,
		1.0f
	};
	pipelineBuilder._scissor = {
		.offset = { 0, 0 },
		.extent = _windowExtent
	};

	VertexInputDescription vertexDescription = Vertex::get_vertex_description();

	// Connect the pipeline builder vertex input info to the one we get from Vertex
	pipelineBuilder._vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();
	pipelineBuilder._vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();

	pipelineBuilder._vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();
	pipelineBuilder._vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();

	pipelineBuilder
		.addVertexShader(meshVertShader)
		.addFragmentShader(colorMeshShader);
	
	std::vector<VkDescriptorSetLayout> setLayouts = { _globalSetLayout, _objectSetLayout };

	auto pipeResult = pipelineBuilder.setLayout(setLayouts, pushConstants);
	VK_UNEXPECTED_ERROR(pipeResult, "Failed to create texture pipe layout")
	VkPipelineLayout meshPipLayout = pipeResult.value();

	// Build the mesh triangle pipeline
	auto pipelineBuild = pipelineBuilder.build_pipeline(DeviceRef(), _renderPass);
	VK_UNEXPECTED_ERROR(pipelineBuild, "Failed to build defaultmesh pipeline")
	VkPipeline meshPipeline = pipelineBuild.value();

	_scene->addMaterial(meshPipeline, meshPipLayout, "defaultmesh");

	pipelineBuilder
		.removeShaders()
		.addVertexShader(meshVertShader)
		.addFragmentShader(texturedMeshShader);

	std::vector<VkDescriptorSetLayout> texturedSetLayouts = { _globalSetLayout, _objectSetLayout,_singleTextureSetLayout };

	pipeResult = pipelineBuilder.setLayout(texturedSetLayouts, pushConstants);
	VK_UNEXPECTED_ERROR(pipeResult, "Failed to create texture pipe layout")
	VkPipelineLayout texturedPipeLayout = pipeResult.value();

	pipelineBuild = pipelineBuilder.build_pipeline(DeviceRef(), _renderPass);
	VK_UNEXPECTED_ERROR(pipelineBuild, "Failed to build texturedmesh pipeline")
	VkPipeline texPipeline = pipelineBuild.value();

	_scene->addMaterial(texPipeline, texturedPipeLayout, "texturedmesh");

	pipelineBuilder
		.removeShaders()
		.addVertexShader(meshVertAnimShader)
		.addFragmentShader(texturedMeshShader);

	pipeResult = pipelineBuilder.setLayout(texturedSetLayouts, pushConstants);
	VK_UNEXPECTED_ERROR(pipeResult, "Failed to create texture pipe layout")
	VkPipelineLayout texturedAnimPipeLayout = pipeResult.value();

	pipelineBuild = pipelineBuilder.build_pipeline(DeviceRef(), _renderPass);
	VK_UNEXPECTED_ERROR(pipelineBuild, "Failed to build texturedmesh pipeline")
	VkPipeline texAnimPipeline = pipelineBuild.value();

	_scene->addMaterial(texAnimPipeline, texturedAnimPipeLayout, "texturedanimmesh");

	vkDestroyShaderModule(DeviceRef(), meshVertShader, nullptr);
	vkDestroyShaderModule(DeviceRef(), meshVertAnimShader, nullptr);
	vkDestroyShaderModule(DeviceRef(), colorMeshShader, nullptr);
	vkDestroyShaderModule(DeviceRef(), texturedMeshShader, nullptr);

	_onEngineShutdown.push_function([=]() {
		vkDestroyPipeline(DeviceRef(), meshPipeline, nullptr);
		vkDestroyPipeline(DeviceRef(), texPipeline, nullptr);
		vkDestroyPipeline(DeviceRef(), texAnimPipeline, nullptr);

		vkDestroyPipelineLayout(DeviceRef(), meshPipLayout, nullptr);
		vkDestroyPipelineLayout(DeviceRef(), texturedPipeLayout, nullptr);
		vkDestroyPipelineLayout(DeviceRef(), texturedAnimPipeLayout, nullptr);
	});

	return 0;
}

tl::expected<VkShaderModule, Error*> VulkanEngine::load_shader_module(const char* filePath) {
	// Open the file with cursor at the end
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		int errorCode = file.bad() | file.fail() << 1 | file.eof() << 2;
		return tl::unexpected(new FileError(errorCode, ErrorMessage("Unable to open shader module at \"{}\"", filePath)));
	}

	// Find what the size of the file is by looking up the location of the cursor
	// Because the cursor is at the end, it gives the size directly in bytes
	size_t fileSize = (size_t)file.tellg();

	// SPIRV expects the buffer to be on uint32, so make sure to reserve a int vector big enough for the entire file
	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

	// Put file cursor at beggining and load the entire file into the buffer
	file.seekg(0).read((char*)buffer.data(), fileSize);
	file.close();

	VkShaderModuleCreateInfo createInfo = vkinit::createinfo::shaderModule(buffer);

	auto shaderResult = vkcommand::createShaderModule(createInfo);
	VK_UNEXPECTED_ERROR(shaderResult, "Failed to create shader module from file {}", filePath);
	return shaderResult.value();
}

tl::expected<int, VulkanError*> VulkanEngine::upload_mesh(Mesh& mesh) {
	const size_t bufferSize = mesh._vertices.size() * sizeof(Vertex);
	// allocate vertex buffer
	auto allocResult = VMAlloc.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	VK_UNEXPECTED_ERROR(allocResult, "Could not create staging buffer")
	AllocatedBuffer stagingBuffer = allocResult.value();

	// copy vertex data
	auto mapResult = VMAlloc.mapBuffer(stagingBuffer);
	VK_UNEXPECTED_ERROR(mapResult, "Failed to map staging buffer");
	void* data = mapResult.value();

	memcpy(data, mesh._vertices.data(), mesh._vertices.size() * sizeof(Vertex));

	VMAlloc.unmapBuffer(stagingBuffer);

	// allocate vertex buffer
	auto allocResult2 = VMAlloc.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	VK_UNEXPECTED_ERROR(allocResult2, "Could not create vertex buffer on GPU")
	mesh._vertexBuffer = allocResult2.value();

	auto submitResult = immediate_submit([=](VkCommandBuffer cmd) {
		VkBufferCopy copy;
		copy.dstOffset = 0;
		copy.srcOffset = 0;
		copy.size = bufferSize;
		vkCmdCopyBuffer(cmd, stagingBuffer._buffer, mesh._vertexBuffer._buffer, 1, &copy);
	});

	if (submitResult) {
		return tl::unexpected(new VulkanError(submitResult.value()->getCode(), submitResult.value(), ErrorMessage("Failed to copy CPU buffer onto GPU")));
	}

	stagingBuffer.destroy();

	return 0;
}

std::optional<VulkanError*> VulkanEngine::draw_objects(VkCommandBuffer cmd) {
	float framed = (_frameNumber / 120.f);

	//_sceneParameters.ambientColor = { sin(framed),0,cos(framed),1 };
	_sceneParameters.ambientColor = { 0,0,0,1 };
	
	auto mapResult = VMAlloc.mapBuffer(_sceneParameterBuffer);
	VK_OPTIONAL_ERROR(mapResult, "Could not map scene data buffer");
	char* sceneData = (char*)mapResult.value();

	int frameIndex = _frameNumber % FRAME_OVERLAP;

	sceneData += pad_uniform_buffer_size(sizeof(GPUSceneData)) * frameIndex;

	memcpy(sceneData, &_sceneParameters, sizeof(GPUSceneData));

	VMAlloc.unmapBuffer(_sceneParameterBuffer);
	
	mapResult = VMAlloc.mapBuffer(thisFrame().objectBuffer);
	VK_OPTIONAL_ERROR(mapResult, "Could not map object buffer");
	
	GPUObjectData* objectSSBO = (GPUObjectData*)mapResult.value();

	int counter = 0;
	for (auto &&[entity, object, transform]: _scene->getSimpleRenders().each()) {
		Object renderObject = _scene->getObject(entity);
		objectSSBO[counter].modelMatrix = transform.getMatrix();
		// Remember counter value as a component
		watch_ptr<SSBOIndex> index = renderObject.addComponent<SSBOIndex>(counter);
		//index->index = counter;
		counter++;
	}
	
	VMAlloc.unmapBuffer(thisFrame().objectBuffer);

	Mesh* lastMesh = nullptr;
	Material* lastMaterial = nullptr;
	
	// for (int i = 0; i < count; i++) {
	for (auto &&[camEntity, camera]: _scene->getCameras().each()) {
		auto mapResult = VMAlloc.mapBuffer(thisFrame().cameraBuffer);
		VK_OPTIONAL_ERROR(mapResult, "Could not map camera buffer");
		void* data = mapResult.value();

		memcpy(data, &camera(), sizeof(GPUCameraData));

		VMAlloc.unmapBuffer(thisFrame().cameraBuffer);

		for (auto &&[entity, object, transform, SSBO]: _scene->getRenders().each()) {
			//only bind the pipeline if it doesnt match with the already bound one
			if (object.material != lastMaterial) {

				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipeline);
				lastMaterial = object.material;
				glm::vec2 cameraViewSize = camera.getViewport();
				VkExtent2D cameraExtent = {static_cast<uint32_t>(cameraViewSize.x), static_cast<uint32_t>(cameraViewSize.y)};
				VkViewport cameraViewport = {
					0.0f,
					0.0f,
					cameraViewSize.x,
					cameraViewSize.y,
					0.0f,
					1.0f
				};
				VkRect2D cameraScissor = {
					.offset = { 0, 0 },
					.extent = cameraExtent
				};
				vkCmdSetViewport(cmd, 0, 1, &cameraViewport);

				vkCmdSetScissor(cmd, 0, 1, &cameraScissor);

				uint32_t uniform_offset = pad_uniform_buffer_size(sizeof(GPUSceneData)) * frameIndex;
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelineLayout, 0, 1, &thisFrame().globalDescriptor, 1, &uniform_offset);
			
				//object data descriptor
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelineLayout, 1, 1, &thisFrame().objectDescriptor, 0, nullptr);

				if (object.material->textureSet != VK_NULL_HANDLE) {
					//texture descriptor
					vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelineLayout, 2, 1, &object.material->textureSet, 0, nullptr);

				}
			}

			glm::mat4 model = transform.getMatrix();
			//final render matrix, that we are calculating on the cpu
			glm::mat4 mesh_matrix = model;

			MeshPushConstants constants;
			constants.data.x = _time;
			constants.render_matrix = mesh_matrix;

			//upload the mesh to the gpu via pushconstants
			vkCmdPushConstants(cmd, object.material->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);

			//only bind the mesh if its a different one from last bind
			if (object.mesh != lastMesh) {
				//bind the mesh vertex buffer with offset 0
				VkDeviceSize offset = 0;
				vkCmdBindVertexBuffers(cmd, 0, 1, &object.mesh->_vertexBuffer._buffer, &offset);
				lastMesh = object.mesh;
			}
			vkCmdDraw(cmd, object.mesh->_vertices.size(), 1,0 , SSBO.index);
		}
	}
	return std::nullopt;
}

size_t VulkanEngine::pad_uniform_buffer_size(size_t originalSize) {
	// Calculate required alignment based on minimum device offset alignment
	size_t minUboAlignment = _gpuProperties.limits.minUniformBufferOffsetAlignment;
	if (minUboAlignment > 0) {
		return (originalSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}
	return originalSize;
}

std::optional<VulkanError*> VulkanEngine::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function) {
	VkCommandBuffer cmd = _uploadContext._commandBuffer;
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	auto operationResult = vkcommand::beginCommandBuffer(cmd, cmdBeginInfo);
	VK_OPTIONAL_OPT_ERROR(operationResult, ErrorMessage("Could not begin command buffer for an immediate submit"))

	function(cmd);

	operationResult = vkcommand::endCommandBuffer(cmd);
	VK_OPTIONAL_OPT_ERROR(operationResult, ErrorMessage("Could not end command buffer for an immediate submit"))

	VkSubmitInfo submit = vkinit::submit_info(&cmd);

	operationResult = vkcommand::singleQueueSubmit(_graphicsQueue, submit, _uploadContext._uploadFence());
	VK_OPTIONAL_OPT_ERROR(operationResult, ErrorMessage("Error making an immediate submit"))

	operationResult = _uploadContext._uploadFence.wait(UINT64_MAX);
	VK_OPTIONAL_OPT_ERROR(operationResult, ErrorMessage("Error while waiting for an immediate submit to finish"))

	operationResult = _uploadContext._uploadFence.reset();
	VK_OPTIONAL_OPT_ERROR(operationResult, ErrorMessage("Could not reset upload fence"))

	operationResult = vkcommand::resetCommandPool(_uploadContext._commandPool, 0);
	VK_OPTIONAL_OPT_ERROR(operationResult, ErrorMessage("Failed to reset command pool after an immediate submit"))

	return std::nullopt;
}

tl::expected<int, Error*> VulkanEngine::initDescriptors() {

	// Create a descriptor pool that will hold 10 uniform buffers
	std::vector<VkDescriptorPoolSize> sizes =
	{
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10 }
	};

	VkDescriptorPoolCreateInfo pool_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.maxSets = 10,
		.poolSizeCount = (uint32_t)sizes.size(),
		.pPoolSizes = sizes.data()
	};

	auto poolResult = vkcommand::createDescriptorPool(&pool_info);
	VK_UNEXPECTED_ERROR(poolResult, "Failed to create default descriptor pool");
	_descriptorPool = poolResult.value();
	
	VkDescriptorSetLayoutBinding cameraBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT,0);
	VkDescriptorSetLayoutBinding sceneBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	
	VkDescriptorSetLayoutBinding bindings[] = { cameraBind,sceneBind };

	VkDescriptorSetLayoutCreateInfo setinfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,

		.flags = 0,
		.bindingCount = 2,
		.pBindings = bindings
	};

	auto descriptorResult = vkcommand::createDescriptorSetLayout(&setinfo);
	VK_UNEXPECTED_ERROR(descriptorResult, "Failed to create descriptor set layout for uniform bindings");
	_globalSetLayout = descriptorResult.value();

	VkDescriptorSetLayoutBinding objectBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);

	VkDescriptorSetLayoutCreateInfo set2info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = 1,
		.pBindings = &objectBind
	};

	descriptorResult = vkcommand::createDescriptorSetLayout(&set2info);
	VK_UNEXPECTED_ERROR(descriptorResult, "Failed to create descriptor set layout for an object");
	_objectSetLayout = descriptorResult.value();

	VkDescriptorSetLayoutBinding textureBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0);

	VkDescriptorSetLayoutCreateInfo set3info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = 1,
		.pBindings = &textureBind
	};

	descriptorResult = vkcommand::createDescriptorSetLayout(&set3info);
	VK_UNEXPECTED_ERROR(descriptorResult, "Failed to create descriptor set layout for a texture");
	_singleTextureSetLayout = descriptorResult.value();

	const size_t sceneParamBufferSize = FRAME_OVERLAP * pad_uniform_buffer_size(sizeof(GPUSceneData));

	auto createResult = VMAlloc.createBuffer(sceneParamBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	VK_UNEXPECTED_ERROR(createResult, "Failed to create a buffer for scene parameters")
	_sceneParameterBuffer = createResult.value();

	_onEngineShutdown.push_function([&]() {
		_sceneParameterBuffer.destroy();

		vkDestroyDescriptorSetLayout(DeviceRef(), _objectSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(DeviceRef(), _globalSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(DeviceRef(), _singleTextureSetLayout, nullptr);

		vkDestroyDescriptorPool(DeviceRef(), _descriptorPool, nullptr);
	});
	return 0;
}

tl::expected<int, Error*> VulkanEngine::initFrames() {
	for (int i = 0; i < FRAME_OVERLAP; i++) {
		auto frameResult = _frames[i].create(_graphicsQueueFamily, _descriptorPool, _globalSetLayout, _objectSetLayout, _sceneParameterBuffer);
		VK_UNEXPECTED_ERROR(frameResult, "Could not create frame {}", i);

		_onEngineShutdown.push_function(frameResult.value().destroySync);
		_onEngineShutdown.push_function(frameResult.value().destroyCommands);
		_onEngineShutdown.push_function(frameResult.value().destroyDescriptors);
	}	

	return 0;
}

tl::expected<VkDescriptorSet, VulkanError*> VulkanEngine::addSingleTextureDescriptor(VkImageView textureView) {
	VkDescriptorSetAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,

		.descriptorPool = _descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &_singleTextureSetLayout
	};

	auto allocateSetsResult = vkcommand::allocateDescriptorSets(&allocInfo);
	VK_UNEXPECTED_ERROR(allocateSetsResult, "Could not allocate descriptor set for a textured material");
	VkDescriptorSet result = allocateSetsResult.value()[0];

	VkSamplerCreateInfo samplerInfo = vkinit::createinfo::sampler(VK_FILTER_NEAREST);

	auto samplerResult = vkcommand::createSampler(samplerInfo);
	VK_UNEXPECTED_ERROR(samplerResult, "Failed to create a sampler for textured material");
	VkSampler blockySampler = samplerResult.value();

	_onEngineShutdown.push_function([=]() {
		vkDestroySampler(DeviceRef(), blockySampler, nullptr);
	});

	VkDescriptorImageInfo imageBufferInfo {
		.sampler = blockySampler,
		.imageView = textureView,
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};

	VkWriteDescriptorSet texture1 = vkinit::write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, result, &imageBufferInfo, 0);

	vkUpdateDescriptorSets(DeviceRef(), 1, &texture1, 0, nullptr);
	return result;
}
