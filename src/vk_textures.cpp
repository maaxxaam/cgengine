#include "src/devicesingleton.h"
#include "src/error.h"
#include "src/vk_engine.h"
#include "src/vk_operations.h"
#include <optional>
#include <vulkan/vulkan_core.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

#include "vk_initializers.h"
#include "vmalloc.h"
#include "vk_textures.h"


tl::expected<AllocatedImage, Error*> vkutil::load_image_from_file(VulkanEngine& engine, const char* fileName) {
	int texWidth, texHeight, texChannels;

	stbi_uc* pixels = stbi_load(fileName, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);	

	if (!pixels) {
		return tl::unexpected(new Error(ErrorMessage("Failed to load texture file {}", fileName)));
	}
	
	void* pixel_ptr = pixels;
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;

	auto bufferResult = VMAlloc.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	VK_UNEXPECTED_ERROR(bufferResult, "Could not create texture buffer")

	AllocatedBuffer stagingBuffer = bufferResult.value();

	auto mapResult = VMAlloc.mapBuffer(stagingBuffer);
	VK_UNEXPECTED_ERROR(mapResult, "Failed to map to staging buffer");
	void* data = mapResult.value();

	memcpy(data, pixel_ptr, static_cast<size_t>(imageSize));

	VMAlloc.unmapBuffer(stagingBuffer);

	stbi_image_free(pixels);

	VkExtent3D imageExtent {
		static_cast<uint32_t>(texWidth),
		static_cast<uint32_t>(texHeight),
		1
	};
	
	VkImageCreateInfo dimg_info = vkinit::createinfo::image(image_format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);

	AllocatedImage newImage;	
	
	VmaAllocationCreateInfo dimg_allocinfo = { .usage = VMA_MEMORY_USAGE_GPU_ONLY };

	//allocate and create the image
	auto imageResult = VMAlloc.createImage(0, VMA_MEMORY_USAGE_GPU_ONLY, dimg_info);
	VK_UNEXPECTED_ERROR(imageResult, "Failed to create image for a texture");
	newImage = imageResult.value();
	
	//transition image to transfer-receiver	
	auto submitResult = engine.immediate_submit([&](VkCommandBuffer cmd) {
		VkImageSubresourceRange range {
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,
			1,
			0,
			1,
		};

		VkImageMemoryBarrier imageBarrier_toTransfer = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,

			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,

			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.image = newImage._image,
			.subresourceRange = range,
		};


		//barrier the image into the transfer-receive layout
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);
		
		VkBufferImageCopy copyRegion = {
			.bufferOffset = 0,
			.bufferRowLength = 0,
			.bufferImageHeight = 0,
			.imageSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.imageExtent = imageExtent
		};

		//copy the buffer into the image
		vkCmdCopyBufferToImage(cmd, stagingBuffer._buffer, newImage._image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

		imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		
		imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		//barrier the image into the shader readable layout
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable);
	});


	engine._onEngineShutdown.push_function([=]() {
		VMAlloc.destroyImage(newImage);
	});

	VMAlloc.destroyBuffer(stagingBuffer);

	std::cout << "Texture " << fileName << " loaded succesfully\n";

	return newImage;
}

std::optional<Error *> TextureAsset::loadRGBAFile(VulkanEngine& engine, const char* filePath) {
	auto image = vkutil::load_image_from_file(engine, "../assets/lost_empire-RGBA.png");
	if (!image.has_value()) {
		return new Error(image.error(), "Failed to load Lost Empire texture!");
	}
	_texture.image = image.value();
	
	VkImageViewCreateInfo imageinfo = vkinit::createinfo::imageView(VK_FORMAT_R8G8B8A8_SRGB, _texture.image._image, VK_IMAGE_ASPECT_COLOR_BIT);
	auto imageViewResult = vkcommand::createImageView(imageinfo);
	if (!imageViewResult)
		return new VulkanError(imageViewResult.error()->getCode(), imageViewResult.error(), ErrorMessage("Failed to create image view for the lostEmpire texture"));
	_texture.imageView = imageViewResult.value();
	_init = true;
	return {};
}

void TextureAsset::unload() {
	vkDestroyImageView(DeviceRef(), _texture.imageView, nullptr);	
	_init = false;
}
