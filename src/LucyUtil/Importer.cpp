#include "Importer.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assert.h>
#include <LucyRE/LucyRE.h>

util::TYPE_MESH LoadMesh(aiMesh* ai_mesh);
util::TYPE_MESH_GPU LoadMeshToGPU(aiMesh* ai_mesh, glm::vec3 offset);

#define ai_importer ImporterInstance()
static Assimp::Importer* ImporterInstance() {
	static Assimp::Importer* instance = nullptr;
	if (instance == nullptr)
		instance = new Assimp::Importer();
	return instance;
}

util::TYPE_MESH util::LoadMesh(const std::string& filename, const std::string& mesh_name) {
	const auto* ai_scene = ai_importer->ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!(ai_scene && !(ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && ai_scene->mRootNode)) {
		return { -2, nullptr, nullptr, nullptr, nullptr, nullptr, 0, nullptr };
	}

	aiMesh* ai_mesh = nullptr;
	for (int i = 0; i < ai_scene->mNumMeshes; i++) {
		ai_mesh = ai_scene->mMeshes[i];
		if (mesh_name == std::string(ai_mesh->mName.C_Str()))
			break;
		ai_mesh = nullptr;
	}

	if (ai_mesh == nullptr) {
		return { -1, nullptr, nullptr, nullptr, nullptr, nullptr, 0, nullptr };
	}


	const auto& tmp = LoadMesh(ai_mesh);

	ai_importer->FreeScene();

	return tmp;
}

util::TYPE_MESH_GPU util::LoadMeshToGPU(const std::string& filename, const std::string& mesh_name, glm::vec3 offset) {
	const auto* ai_scene = ai_importer->ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!(ai_scene && !(ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && ai_scene->mRootNode)) {
		return { nullptr, nullptr, -2, nullptr, 0 };
	}

	aiMesh* ai_mesh = nullptr;
	for (int i = 0; i < ai_scene->mNumMeshes; i++) {
		ai_mesh = ai_scene->mMeshes[i];
		if (mesh_name == std::string(ai_mesh->mName.C_Str()))
			break;
		ai_mesh = nullptr;
	}

	if (ai_mesh == nullptr) {
		return { nullptr, nullptr, -1, nullptr, 0 };
	}

	const auto& tmp = LoadMeshToGPU(ai_mesh, offset);

	ai_importer->FreeScene();

	return tmp;
}

inline util::TYPE_MESH LoadMesh(aiMesh* ai_mesh) {
	int indexcount = 0;
	int vertexcount = ai_mesh->mNumVertices;

	util::position_array* position_array = nullptr;
	util::normal_array* normal_array = nullptr;
	util::color_2d_array* color_array = nullptr;
	util::uv_2d_array* uv_array = nullptr;
	util::uvw_2d_array* uvw_array = nullptr;
	uint32_t* indices = nullptr;

	if (vertexcount > 0) {
		if (ai_mesh->HasPositions())
			position_array = new util::position_array[vertexcount];
		if (ai_mesh->HasNormals())
			normal_array = new util::normal_array[vertexcount];

		for (int i = 0; i < ai_mesh->GetNumColorChannels(); i++) {
			if (color_array == nullptr)
				color_array = new util::color_2d_array[AI_MAX_NUMBER_OF_COLOR_SETS];

			color_array[i] = new util::color_array[vertexcount];
		}

		for (int i = 0; i < 8; i++) {
			if (ai_mesh->mNumUVComponents[i] == 2) {
				if (uv_array == nullptr) {
					uv_array = new util::uv_2d_array[8];

					for (int idx = 0; idx < 8; idx++)
						uv_array[idx] = nullptr;
				}

				uv_array[i] = new util::uv_array[vertexcount];
			}
			if (ai_mesh->mNumUVComponents[i] == 3) {
				if (uvw_array == nullptr) {
					uvw_array = new util::uvw_2d_array[8];

					for (int idx = 0; idx < 8; idx++)
						uvw_array[idx] = nullptr;
				}

				uvw_array[i] = new util::uvw_array[vertexcount];
			}
		}

		for (int i = 0; i < vertexcount; i++) {
			if (position_array != nullptr) {
				position_array[i] = *(glm::vec3*)&ai_mesh->mVertices[i];
			}
			if (normal_array != nullptr) {
				normal_array[i] = *(glm::vec3*)&ai_mesh->mVertices[i];
			}
			if (color_array != nullptr) {
				for (int idx = 0; idx < 8; idx++) {
					if (color_array[idx] == nullptr)
						break;
					color_array[idx][i] = *(glm::vec4*)&ai_mesh->mColors[idx][i];
				}
			}
			if (uv_array != nullptr) {
				for (int idx = 0; idx < 8; idx++)
					if (uv_array[idx] != nullptr)
						uv_array[idx][i] = *(glm::vec2*)&ai_mesh->mTextureCoords[idx][i];
			}
			if (uvw_array != nullptr) {
				for (int idx = 0; idx < 8; idx++)
					if (uvw_array[idx] != nullptr)
						uvw_array[idx][i] = *(glm::vec3*)&ai_mesh->mTextureCoords[idx][i];
			}
		}

		if (ai_mesh->HasFaces()) {
			for (int i = 0; i < ai_mesh->mNumFaces; i++)
				indexcount += ai_mesh->mFaces[i].mNumIndices;

			indices = new uint32_t[indexcount];

			int idx = 0;
			for (int i = 0; i < ai_mesh->mNumFaces; i++) {
				aiFace face = ai_mesh->mFaces[i];

				for(int j = 0; j < face.mNumIndices; j++)
					indices[idx++] = face.mIndices[j];
			}
		}
	}

	return { vertexcount, position_array, normal_array, color_array, uv_array, uvw_array, indexcount, indices };
}

inline util::TYPE_MESH_GPU LoadMeshToGPU(aiMesh* ai_mesh, glm::vec3 offset) {
	int vertexcount = ai_mesh->mNumVertices;
	int indexcount = 0;

	if (vertexcount <= 0) return { nullptr, nullptr, 0, nullptr, 0 };

	uint32_t flag = 0;
	if (ai_mesh->HasPositions())
		flag |= lre::Vertex::position_flag;
	if (ai_mesh->HasNormals())
		flag |= lre::Vertex::normal_flag;
	if (ai_mesh->HasVertexColors(0))
		flag |= lre::Vertex::color_flag;
	if (ai_mesh->HasVertexColors(0))
		flag |= lre::Vertex::uv_flag;
	if (ai_mesh->HasVertexColors(0))
		flag |= lre::Vertex::uvw_flag;

	auto& vertex_offset = lre::Vertex::OFFSET[flag];
	int stride = vertex_offset[lre::Vertex::STRIDE];

	if (stride == 0) return { nullptr, nullptr, 0, nullptr, 0 };

	float* vertices = new float[vertexcount * stride * sizeof(float)];

	for (int i = 0; i < vertexcount; i++) {
		if (vertex_offset[lre::Vertex::position] >= 0) {
			if (offset != glm::vec3(0, 0, 0)) {
				*(glm::vec3*)(&vertices[i * stride + vertex_offset[lre::Vertex::position]]) = *(glm::vec3*)(&ai_mesh->mVertices[i]) + offset;
			} else {
				*(glm::vec3*)(&vertices[i * stride + vertex_offset[lre::Vertex::position]]) = *(glm::vec3*)(&ai_mesh->mVertices[i]);
			}
		}
		if (vertex_offset[lre::Vertex::normal] >= 0) {
			*(glm::vec3*)(&vertices[i * stride + vertex_offset[lre::Vertex::normal]]) = *(glm::vec3*)(&ai_mesh->mNormals[i]);
		}
		if (vertex_offset[lre::Vertex::color] >= 0) {
			*(glm::vec4*)(&vertices[i * stride + vertex_offset[lre::Vertex::color]]) = *(glm::vec4*)(&ai_mesh->mColors[0][i]);
		}
		if (vertex_offset[lre::Vertex::uv] >= 0) {
			*(glm::vec2*)(&vertices[i * stride + vertex_offset[lre::Vertex::uv]]) = *(glm::vec2*)(&ai_mesh->mTextureCoords[0][i]);
		}
		// if (vertex_offset[lre::Vertex::uvw] >= 0) {
		// 	*(glm::vec3*)(&vertices[i * stride + vertex_offset[lre::Vertex::uvw]]) = *(glm::vec3*)(&uvw[i]);
		// }
	}

	lgl::VertexBuffer* vertexbuffer = nullptr;
	lgl::VertexArray* vertexarray = nullptr;
	lgl::IndexBuffer* indexbuffer = nullptr;

	vertexarray = lre::Vertex::MAP[flag]();
	
	if (ai_mesh->HasFaces()) {
		for (int i = 0; i < ai_mesh->mNumFaces; i++)
			indexcount += ai_mesh->mFaces[i].mNumIndices;

		auto* indices = new uint32_t[indexcount];

		int idx = 0;
		for (int i = 0; i < ai_mesh->mNumFaces; i++) {
			aiFace face = ai_mesh->mFaces[i];

			for(int j = 0; j < face.mNumIndices; j++)
				indices[idx++] = face.mIndices[j];
		}

		if (indexbuffer == nullptr)
			indexbuffer = new lgl::IndexBuffer();

		indexbuffer->Bind();
		indexbuffer->Allocate(sizeof(uint32_t) * indexcount);
		indexbuffer->AddData((void*)indices, sizeof(uint32_t) * indexcount);
		indexbuffer->UnBind();
	}

	if (vertexbuffer == nullptr)
		vertexbuffer = new lgl::VertexBuffer();

	vertexbuffer->Bind();
	vertexbuffer->Allocate(vertexcount * stride * sizeof(float));
	vertexbuffer->AddDataDynamic(vertices, vertexcount * stride * sizeof(float));
	vertexbuffer->UnBind();

	delete[] vertices;

	return { vertexarray, vertexbuffer, vertexcount, indexbuffer, indexcount };
}

void util::Free(TYPE_MESH& mesh) {
	auto& [vertexcount, position_array, normal_array, color_array, uv_array, uvw_array, indexcount, indices] = mesh;

	if (position_array != nullptr) {
		delete[] position_array;
		position_array = nullptr;
	}
	if (normal_array != nullptr) {
		delete[] normal_array;
		normal_array = nullptr;
	}
	if (color_array != nullptr) {
		for (int i = 0; i < 8; i++) {
			if (color_array[i] != nullptr) {
				delete[] color_array[i];
				color_array[i] = nullptr;
			}
		}

		delete[] color_array;
		color_array = nullptr;
	}
	if (uv_array != nullptr) {
		for (int i = 0; i < 8; i++) {
			if (uv_array[i] != nullptr) {
				delete[] uv_array[i];
				uv_array[i] = nullptr;
			}
		}

		delete[] uv_array;
		uv_array = nullptr;
	}
	if (uvw_array != nullptr) {
		for (int i = 0; i < 8; i++) {
			if (uvw_array[i] != nullptr) {
				delete[] uvw_array[i];
				uvw_array[i] = nullptr;
			}
		}

		delete[] uvw_array;
		uvw_array = nullptr;
	}
	if (indices != nullptr) {
		delete[] indices;
		indices = nullptr;
	}

	vertexcount = 0;
	indexcount = 0;
}
