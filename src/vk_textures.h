// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "error.h"
#include "allocstructs.h"
#include "material.h"

class VulkanEngine;

struct Texture {
	AllocatedImage image;
	VkImageView imageView;
};

namespace vkutil {
	tl::expected<AllocatedImage, Error*> load_image_from_file(VulkanEngine& engine, const char* file);
}

class TextureAsset {
public:
	std::optional<Error *> loadRGBAFile(VulkanEngine& engine, const char* filePath);
	tl::expected<Material, Error*> createSimpleMaterial(VulkanEngine& engine, Material baseMaterial);
	void unload();
	bool isInitialized() { return _init; };
	const Texture& getData() const { assert(_init); return _texture; };
private:
	bool _init = false;
	Texture _texture;
};
