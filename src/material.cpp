#include <vulkan/vulkan_core.h>

#include "src/vk_operations.h"
#include "material.h"

PipelineBuilder::PipelineBuilder() {
    _multisampling = multisamplingState();
    _vertexInputInfo = vertexInputState();
    _inputAssembly = inputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    _rasterizer = rasterizationState(VK_POLYGON_MODE_FILL);

   	// A single blend attachment with no blending and writing to RGBA
	_colorBlendAttachment = colorBlendAttachmentState();
	_depthStencil = depthStencil(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
}

PipelineBuilder& PipelineBuilder::addFragmentShader(VkShaderModule& shader) {
    _shaderStages.push_back(pipelineShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, shader));
    return *this;
}

PipelineBuilder& PipelineBuilder::addVertexShader(VkShaderModule& shader) {
    _shaderStages.push_back(pipelineShaderStage(VK_SHADER_STAGE_VERTEX_BIT, shader));
    return *this;
}

PipelineBuilder& PipelineBuilder::removeShaders() {
    _shaderStages.clear();
    return *this;
}

tl::expected<VkPipelineLayout, VulkanError*> PipelineBuilder::setLayout(std::vector<VkDescriptorSetLayout>& setLayouts, std::vector<VkPushConstantRange>& pushConstants) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = pipelineLayout(setLayouts, pushConstants);

	auto pipeLayoutResult = vkcommand::createPipelineLayout(pipelineLayoutInfo);
	VK_UNEXPECTED_ERROR(pipeLayoutResult, "Failed to create pipeline layout");
	VkPipelineLayout pipeLayout = pipeLayoutResult.value();

	_pipelineLayout = pipeLayout;
    return _pipelineLayout;
}

tl::expected<VkPipeline, VulkanError*> PipelineBuilder::build_pipeline(VkDevice device, VkRenderPass pass) {
	// make viewport state from our stored viewport and scissor.
	// at the moment we wont support multiple viewports or scissors
	VkPipelineViewportStateCreateInfo viewportState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = nullptr,

		.viewportCount = 1,
		.pViewports = &_viewport,
		.scissorCount = 1,
		.pScissors = &_scissor
	};

	// setup dummy color blending. We arent using transparent objects yet
	// the blending is just "no blend", but we do write to the color attachment
	VkPipelineColorBlendStateCreateInfo colorBlending = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = nullptr,

		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &_colorBlendAttachment,
	};

	std::vector<VkDynamicState> stateVars = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pNext = nullptr,

		.dynamicStateCount = static_cast<uint32_t>(stateVars.size()),
		.pDynamicStates = stateVars.data(),
	};

	// build the actual pipeline
	// we now use all of the info structs we have been writing into into this one to create the pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = nullptr,

		.stageCount = static_cast<uint32_t>(_shaderStages.size()),
		.pStages = _shaderStages.data(),
		.pVertexInputState = &_vertexInputInfo,
		.pInputAssemblyState = &_inputAssembly,
		.pViewportState = &viewportState,
		.pRasterizationState = &_rasterizer,
		.pMultisampleState = &_multisampling,
		.pDepthStencilState = &_depthStencil,
		.pColorBlendState = &colorBlending,
		.pDynamicState = &dynamicState,
		.layout = _pipelineLayout,
		.renderPass = pass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE
	};

	auto pipeResult = vkcommand::createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo);
	VK_UNEXPECTED_ERROR(pipeResult, "Failed to create default graphics pipeline");
	return pipeResult.value();
}

VkPipelineShaderStageCreateInfo PipelineBuilder::pipelineShaderStage(VkShaderStageFlagBits stage, VkShaderModule shaderModule) {
	return VkPipelineShaderStageCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = nullptr,

		//shader stage
		.stage = stage,
		//module containing the code for this shader stage
		.module = shaderModule,
		//the entry point of the shader
		.pName = "main"
	};
}

VkPipelineVertexInputStateCreateInfo PipelineBuilder::vertexInputState() {
	return VkPipelineVertexInputStateCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = nullptr,
		//no vertex bindings or attributes
		.vertexBindingDescriptionCount = 0,
		.vertexAttributeDescriptionCount = 0
	};
}

VkPipelineInputAssemblyStateCreateInfo PipelineBuilder::inputAssembly(VkPrimitiveTopology topology) {
	return VkPipelineInputAssemblyStateCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext = nullptr,

		.topology = topology,
		//we are not going to use primitive restart on the entire tutorial so leave it on false
		.primitiveRestartEnable = VK_FALSE
	};
}

VkPipelineRasterizationStateCreateInfo PipelineBuilder::rasterizationState(VkPolygonMode polygonMode) {
	return VkPipelineRasterizationStateCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.pNext = nullptr,

		.depthClampEnable = VK_FALSE,
		//rasterizer discard allows objects with holes, default to no
		.rasterizerDiscardEnable = VK_FALSE,

		.polygonMode = polygonMode,
		//no backface cull
		.cullMode = VK_CULL_MODE_NONE,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		//no depth bias
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0f,
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = 0.0f,
		.lineWidth = 1.0f
	};
}

VkPipelineMultisampleStateCreateInfo PipelineBuilder::multisamplingState() {
	return VkPipelineMultisampleStateCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.pNext = nullptr,
		//multisampling defaulted to no multisampling (1 sample per pixel)
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 1.0f,
		.pSampleMask = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE
	};
}

VkPipelineLayoutCreateInfo PipelineBuilder::pipelineLayout(std::vector<VkDescriptorSetLayout>& setLayouts, std::vector<VkPushConstantRange>& constantRanges) {
	return VkPipelineLayoutCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,

		.flags = 0,
		.setLayoutCount = static_cast<uint32_t>(setLayouts.size()),
		.pSetLayouts = setLayouts.data(),
		.pushConstantRangeCount = static_cast<uint32_t>(constantRanges.size()),
		.pPushConstantRanges = constantRanges.data(),
	};
}

VkPipelineColorBlendAttachmentState PipelineBuilder::colorBlendAttachmentState() {
	return VkPipelineColorBlendAttachmentState {
		.blendEnable = VK_FALSE,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};
}

VkPipelineDepthStencilStateCreateInfo PipelineBuilder::depthStencil(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp) {
	return VkPipelineDepthStencilStateCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.pNext = nullptr,

		.depthTestEnable = bDepthTest ? VK_TRUE : VK_FALSE,
		.depthWriteEnable = bDepthWrite ? VK_TRUE : VK_FALSE,
		.depthCompareOp = bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = VK_FALSE,
		// Optional
		.minDepthBounds = 0.0f,
		.maxDepthBounds = 1.0f
	};
}
