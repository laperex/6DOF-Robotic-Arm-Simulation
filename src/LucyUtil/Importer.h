#pragma once

#include <glm/glm.hpp>
#include <tuple>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <LucyGL/LucyGL.h>

namespace util {
	typedef glm::vec3 position_array;
	typedef glm::vec3 normal_array;
	typedef glm::vec4 color_array;
	typedef glm::vec2 uv_array;
	typedef glm::vec3 uvw_array;

	typedef color_array* color_2d_array;
	typedef uv_array* uv_2d_array;
	typedef uvw_array* uvw_2d_array;

	// auto [vertexcount, positions, normals, colors, uvs, uvws, indexcount, indices]
	typedef std::tuple<int, position_array*, normal_array*, color_2d_array*, uv_2d_array*, uvw_2d_array*, int, uint32_t*> TYPE_MESH;
	typedef std::tuple<lgl::VertexArray*, lgl::VertexBuffer*, int, lgl::IndexBuffer*, int> TYPE_MESH_GPU;

	// // auto [vertexcount, positions, normals, colors, uvs, uvws, indexcount, indices]
	// typedef std::tuple<int, position_array*, normal_array*, col_2d_array*, uv_2d_array*, uvw_2d_array*, int, uint32_t*> TYPE_MATERIAL;

	TYPE_MESH LoadMesh(const std::string& filename, const std::string& mesh_name);
	util::TYPE_MESH_GPU LoadMeshToGPU(const std::string& filename, const std::string& mesh_name, glm::vec3 offset = glm::vec3(0, 0, 0));
	std::vector<std::pair<std::string, TYPE_MESH>> LoadMeshAll(const std::string& filename);

	void Free(TYPE_MESH& mesh);
	void Free(TYPE_MESH_GPU& mesh);
}
