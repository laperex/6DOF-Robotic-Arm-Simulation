#pragma once

#include <LucyUtil/LucyUtil.h>
#include "Vertex.h"
#include <string>
#include <vector>
#include <memory>
#include <LucyGL/LucyGL.h>
#include <glm/glm.hpp>

namespace lre {
	struct Renderer {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
		glm::vec3 view_position;

		std::unique_ptr<lgl::UniformBuffer> uniformbuffer;

		lgl::FrameBuffer* framebuffer = nullptr;
		lgl::Shader* shader = nullptr;
		lgl::VertexArray* vertexarray = nullptr;
		lgl::VertexBuffer* vertexbuffer = nullptr;
		lgl::IndexBuffer* indexbuffer = nullptr;

		std::unordered_map<std::string, lgl::Shader*> shader_registry;

		std::unordered_map<UTIL_UUID, std::pair<std::string, util::TYPE_MESH_GPU>> mesh_registry;

		static Renderer* Instance() {
			static Renderer instance;
			return &instance;
		}
	};

	void SetModel(const glm::mat4& model);
	void SetView(const glm::mat4& view);
	void SetProjection(const glm::mat4& projection);
	void SetViewPosition(const glm::vec3& view_position);

	void Clear(const glm::vec4& clearcolor, uint32_t flags);

	void Initialize();
	void InitializeMainShaders();

	void PrepareShadersForRendering();

	void SetFrameBuffer(lgl::FrameBuffer* framebuffer);
	void SetShader(lgl::Shader* shader);
	void SetVertexArray(lgl::VertexArray* vertexarray);
	void SetVertexBuffer(lgl::VertexBuffer* vertexbuffer);
	void SetIndexBuffer(lgl::IndexBuffer* indexbuffer);

	void InsertShader(std::string name, lgl::Shader* shader);
	lgl::Shader* GetShader(std::string name);

	void RenderFrameBufferToScreen(lgl::FrameBuffer* framebuffer, const glm::vec2& size);

	UTIL_UUID InsertMesh(std::string name, const util::TYPE_MESH_GPU& mesh, UTIL_UUID id = UTIL_GENERATE_UUID);

	void Render(lgl::Primitive primitive, lgl::Shader* shader, lgl::VertexArray* vertexarray, lgl::VertexBuffer* vertexbuffer, lgl::IndexBuffer* indexbuffer, int indexcount);
	void Render(lgl::Primitive primitive, lgl::Shader* shader, lgl::VertexArray* vertexarray, lgl::VertexBuffer* vertexbuffer, lgl::IndexBuffer* indexbuffer, int indexcount, int picking_data);
	void Render(lgl::Primitive primitive, lgl::Shader* shader, lgl::VertexArray* vertexarray, lgl::VertexBuffer* vertexbuffer, int vertexcount);
	void Render(lgl::Primitive primitive, lgl::Shader* shader, lgl::VertexArray* vertexarray, lgl::VertexBuffer* vertexbuffer, int vertexcount, int picking_data);

	void RenderMesh(UTIL_UUID id, lgl::Shader* shader);
	void RenderMesh(UTIL_UUID id, lgl::Shader* shader, int picking_data);

	template <typename T>
	void RenderLine(lgl::Primitive primitive, lgl::VertexArray* vertexarray, const T* vertices, int vertexcount) {
		if (vertexcount <= 0) return;
		assert(vertexarray != nullptr);

		static lgl::VertexBuffer* vertexbuffer = nullptr;

		if (vertexbuffer == nullptr) {
			vertexbuffer = new lgl::VertexBuffer();
		}

		vertexbuffer->Bind();
		vertexbuffer->Allocate(vertexcount * sizeof(T));
		vertexbuffer->AddDataDynamic((void*)vertices, vertexcount * sizeof(T));

		vertexarray->Bind();
		vertexarray->BindVertexBuffer(vertexbuffer, vertexarray->stride);

		Render(primitive, nullptr, vertexarray, vertexbuffer, vertexcount);
	}

	template <typename T>
	void RenderLine(lgl::Primitive primitive, const T* vertices, int vertexcount) {
		RenderLine(primitive, T::VertexArray(), vertices, vertexcount);
	}

	void RenderLine(const std::vector<Vertex::P1>& vertices, const glm::vec4& color);
	void RenderLine(const std::vector<glm::vec3>& vertices, const glm::vec4& color);
	void RenderLine(const std::vector<Vertex::P1C1>& vertices);
	void RenderLine(const glm::vec3& v0, const glm::vec3& v1, const glm::vec4& color);
	void FlushLine();

	void RenderLineStrip(const std::vector<Vertex::P1>& vertices, const glm::vec4& color);
	void RenderLineStrip(const std::vector<Vertex::P1C1>& vertices);

	enum Plane {
		XY,
		XZ,
		YZ,
	};

	void RenderGrid(Plane plane, int count, int grid_size, const glm::vec4& color);
	void Test();
	void GridTest(lgl::FrameBuffer* framebuffer, glm::vec2 size);

	void Destroy();
}
