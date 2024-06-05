#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

#include "error.h"

struct Material {
	VkDescriptorSet textureSet{VK_NULL_HANDLE};
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
};

class PipelineBuilder {
public:
    PipelineBuilder();

	std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
	VkViewport _viewport;
	VkRect2D _scissor;
	VkPipelineRasterizationStateCreateInfo _rasterizer;
	VkPipelineColorBlendAttachmentState _colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo _multisampling;
	VkPipelineLayout _pipelineLayout;
	VkPipelineDepthStencilStateCreateInfo _depthStencil;

    PipelineBuilder& addFragmentShader(VkShaderModule& shader);
    PipelineBuilder& addVertexShader(VkShaderModule& shader);
    PipelineBuilder& removeShaders();

    tl::expected<VkPipelineLayout, VulkanError*> setLayout(std::vector<VkDescriptorSetLayout>& setLayouts, std::vector<VkPushConstantRange>& pushConstants);

	tl::expected<VkPipeline, VulkanError*> build_pipeline(VkDevice device, VkRenderPass pass);

private:
    //
    // Struct creations
    VkPipelineShaderStageCreateInfo pipelineShaderStage(VkShaderStageFlagBits stage, VkShaderModule shaderModule);

    VkPipelineVertexInputStateCreateInfo vertexInputState();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly(VkPrimitiveTopology topology);

    VkPipelineRasterizationStateCreateInfo rasterizationState(VkPolygonMode polygonMode);

    VkPipelineMultisampleStateCreateInfo multisamplingState();

    VkPipelineLayoutCreateInfo pipelineLayout(std::vector<VkDescriptorSetLayout>& setLayouts, std::vector<VkPushConstantRange>& constantRanges);

	VkPipelineColorBlendAttachmentState colorBlendAttachmentState();

	VkPipelineDepthStencilStateCreateInfo depthStencil(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp);

};

