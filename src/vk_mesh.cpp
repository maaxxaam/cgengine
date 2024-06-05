#include <tiny_obj_loader.h>

#include <iostream>

#include "vk_mesh.h"

VertexInputDescription Vertex::get_vertex_description() {
	VertexInputDescription description;

	// We will have just 1 vertex buffer binding, with a per-vertex rate
	VkVertexInputBindingDescription mainBinding = {
		.binding = 0,
		.stride = sizeof(Vertex),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};

	description.bindings.push_back(mainBinding);

	VkVertexInputAttributeDescription currentAttribute = {
		.location = 0,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Vertex, position)
	};
	description.attributes.push_back(currentAttribute);

	currentAttribute.location = 1;
	currentAttribute.offset = offsetof(Vertex, normal);
	// Retain binding and format
	description.attributes.push_back(currentAttribute);

	currentAttribute.location = 2;
	currentAttribute.offset = offsetof(Vertex, color);
	// Retain binding and format
	description.attributes.push_back(currentAttribute);

	currentAttribute.location = 3;
	currentAttribute.format = VK_FORMAT_R32G32_SFLOAT;
	currentAttribute.offset = offsetof(Vertex, uv);
	// Retain binding only
	description.attributes.push_back(currentAttribute);
	return description;
}

tl::expected<Mesh, Error*> meshFromOBJ(const char* fileName) {
	tinyobj::attrib_t vertexArrays;
	std::vector<tinyobj::shape_t> objectInfo;
	// materials contains the information about the material of each shape, not used yet.
	std::vector<tinyobj::material_t> materials;

	//error and warning output from the load function
	std::string warn, err;

	tinyobj::LoadObj(&vertexArrays, &objectInfo, &materials, &warn, &err, fileName,
		nullptr);
	
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << "\n";
	}

	if (!err.empty()) {
		return tl::unexpected(new Error(ErrorMessage(err)));
	}

	Mesh result{};

	// Loop over shapes
	for (size_t s = 0; s < objectInfo.size(); s++) {
		size_t index_offset = 0;
		
		// Loop over faces(polygon)
		for (size_t f = 0; f < objectInfo[s].mesh.num_face_vertices.size(); f++) {

			// Only support loading triangles
			// TODO: support more than 3 faces per poly
			int fv = 3;

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				Vertex new_vert;

				// access to vertex
				tinyobj::index_t idx = objectInfo[s].mesh.indices[index_offset + v];

				// Load position
				int stride = 3 * idx.vertex_index;
				new_vert.position = {
					vertexArrays.vertices[stride],
					vertexArrays.vertices[stride + 1],
					vertexArrays.vertices[stride + 2]
				};

				// Load normal
				stride = 3 * idx.normal_index;
				new_vert.normal = {
					vertexArrays.normals[stride],
					vertexArrays.normals[stride + 1],
					vertexArrays.normals[stride + 2]
				};

				// Load UV coordinates
				stride = idx.texcoord_index << 1;
				new_vert.uv = {
					vertexArrays.texcoords[stride],
					1 - vertexArrays.texcoords[stride + 1]
				};

				//we are setting the vertex color as the vertex normal. This is just for display purposes
				new_vert.color = new_vert.normal;

				result._vertices.push_back(new_vert);
			}
			index_offset += fv;
		}
	}

	return result;
}