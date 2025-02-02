#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "eastloverloads.h"
#include <EASTL/vector.h>

#include "allocstructs.h"
#include "error.h"

struct VertexInputDescription {
	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;

	VkPipelineVertexInputStateCreateFlags flags = 0;
};

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 uv;
	static VertexInputDescription get_vertex_description();
}; 

struct Mesh {
	eastl::vector<Vertex> _vertices;

	AllocatedBuffer _vertexBuffer;
};

tl::expected<Mesh, Error*> meshFromOBJ(const char* filename);
