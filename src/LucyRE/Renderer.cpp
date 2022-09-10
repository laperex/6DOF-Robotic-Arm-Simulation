#include "Renderer.h"
#include "Vertex.h"
#include <iostream>

#define self lre::Renderer::Instance()

#include <filesystem>

// #define SHADER_PATH(rel_filepath) std::filesystem::current_path().string() + "\\Shaders\\" + std::string(rel_filepath)
#define SHADER_PATH(rel_filepath) "D:\\C++\\6DOF Arm Simulation\\src\\LucyRE\\Shaders\\" + std::string(rel_filepath)

void lre::SetModel(const glm::mat4& model) {
	if (self->model == model) return;
	self->model = model;
	self->uniformbuffer->AddDataDynamic(&self->model[0][0], sizeof(glm::mat4), sizeof(glm::mat4) * 0);
}

void lre::SetView(const glm::mat4& view) {
	if (self->view == view) return;
	self->view = view;
	self->uniformbuffer->AddDataDynamic(&self->view[0][0], sizeof(glm::mat4), sizeof(glm::mat4) * 1);
}

void lre::SetProjection(const glm::mat4& projection) {
	if (self->projection == projection) return;
	self->projection = projection;
	self->uniformbuffer->AddDataDynamic(&self->projection[0][0], sizeof(glm::mat4), sizeof(glm::mat4) * 2);
}

void lre::SetViewPosition(const glm::vec3& view_position) {
	if (self->view_position == view_position) return;
	self->view_position = view_position;
	self->uniformbuffer->AddDataDynamic(&self->view_position[0], sizeof(glm::vec3), sizeof(glm::mat4) * 3);
}

void lre::Clear(const glm::vec4& clearcolor, uint32_t flags) {
	lgl::ClearColor(clearcolor.x, clearcolor.y, clearcolor.z, clearcolor.w);
	lgl::Clear(flags);
}

void lre::Initialize() {
	assert(lgl::IsInitialized());

	self->uniformbuffer = std::make_unique<lgl::UniformBuffer>();

	self->uniformbuffer->Bind();
	self->uniformbuffer->Allocate(sizeof(glm::mat4) * 4);
	self->uniformbuffer->BindRange(0, sizeof(glm::mat4) * 4, 0);

	SetModel(glm::mat4(1.0f));
	SetView(glm::mat4(1.0f));
	SetProjection(glm::mat4(1.0f));

	InitializeMainShaders();
}

void lre::InitializeMainShaders() {
	assert(lgl::IsInitialized());

	lgl::Shader* screen = new lgl::Shader();
	screen->VertexShader(SHADER_PATH("screen_tex.vs"));
	screen->FragmentShader(SHADER_PATH("uv.fs"));
	screen->Link();
	self->shader_registry["screen"] = screen;

	lgl::Shader* const_color = new lgl::Shader();
	const_color->VertexShader(SHADER_PATH("vertex.vs"));
	const_color->FragmentShader(SHADER_PATH("const_color.fs"));
	const_color->Link();
	self->shader_registry["const_color"] = const_color;

	lgl::Shader* color = new lgl::Shader();
	color->VertexShader(SHADER_PATH("vertex.vs"));
	color->FragmentShader(SHADER_PATH("color.fs"));
	color->Link();
	self->shader_registry["color"] = color;

	lgl::Shader* normal = new lgl::Shader();
	normal->VertexShader(SHADER_PATH("vertex.vs"));
	normal->FragmentShader(SHADER_PATH("normal.fs"));
	normal->Link();
	self->shader_registry["normal"] = normal;

	lgl::Shader* uv = new lgl::Shader();
	uv->VertexShader(SHADER_PATH("vertex.vs"));
	uv->FragmentShader(SHADER_PATH("uv.fs"));
	uv->Link();
	self->shader_registry["uv"] = uv;

	lgl::Shader* uvw = new lgl::Shader();
	uvw->VertexShader(SHADER_PATH("vertex.vs"));
	uvw->FragmentShader(SHADER_PATH("uvw.fs"));
	uvw->Link();
	self->shader_registry["uvw"] = uvw;

	lgl::Shader* phong = new lgl::Shader();
	phong->VertexShader(SHADER_PATH("vertex.vs"));
	phong->FragmentShader(SHADER_PATH("phong.fs"));
	phong->Link();
	self->shader_registry["phong"] = phong;
}

void lre::SetFrameBuffer(lgl::FrameBuffer* framebuffer) {
	self->framebuffer = framebuffer;
}
void lre::SetShader(lgl::Shader* shader) {
	self->shader = shader;
}
void lre::SetVertexArray(lgl::VertexArray* vertexarray) {
	self->vertexarray = vertexarray;
}
void lre::SetVertexBuffer(lgl::VertexBuffer* vertexbuffer) {
	self->vertexbuffer = vertexbuffer;
}
void lre::SetIndexBuffer(lgl::IndexBuffer* indexbuffer) {
	self->indexbuffer = indexbuffer;
}

void lre::InsertShader(std::string name, lgl::Shader* shader) {
	assert(self->shader_registry.find(name) == self->shader_registry.end());

	self->shader_registry[name] = shader;
}

lgl::Shader* lre::GetShader(std::string name) {
	if (self->shader_registry.find(name) == self->shader_registry.end())
		return nullptr;

	return self->shader_registry[name];
}

void lre::RenderFrameBufferToScreen(lgl::FrameBuffer* framebuffer, const glm::vec2& size) {
	framebuffer->UnBind();
	auto* shader = GetShader("screen");

	shader->Bind();
	glm::vec2 res_uv = { framebuffer->width / size.x, framebuffer->height / size.y };
	shader->SetUniformVec2("res_uv", &res_uv[0]);
	shader->SetUniformi("u_texture", 0);

	framebuffer->texture->BindUnit(0);

	static auto* vertexarray = new lgl::VertexArray({});
	vertexarray->Bind();

	lgl::Draw(lgl::TRIANGLE, 0, 6);

	vertexarray->UnBind();
	shader->UnBind();
}

UTIL_UUID lre::InsertMesh(std::string name, const util::TYPE_MESH_GPU& mesh, UTIL_UUID id) {
	assert(self->mesh_registry.find(id) == self->mesh_registry.end());

	self->mesh_registry[id] = { name, mesh };

	return id;
}

void lre::Render(lgl::Primitive primitive, lgl::Shader* shader, lgl::VertexArray* vertexarray, lgl::VertexBuffer* vertexbuffer, lgl::IndexBuffer* indexbuffer, int indexcount) {
	assert(indexbuffer != nullptr && vertexarray != nullptr);
	if (indexcount == 0) return;

	if (shader != nullptr)
		shader->Bind();

	vertexarray->Bind();
	vertexarray->BindVertexBuffer(vertexbuffer, vertexarray->stride);
	vertexarray->BindIndexBuffer(indexbuffer);

	lgl::DrawIndexed(lgl::TRIANGLE, indexcount, lgl::UNSIGNED_INT, nullptr);
}

void lre::RenderMesh(UTIL_UUID id) {
	assert(self->mesh_registry.find(id) != self->mesh_registry.end());

	auto [ vertexarray, vertexbuffer, vertexcount, indexbuffer, indexcount] = self->mesh_registry[id].second;

	Render(lgl::TRIANGLE, nullptr, vertexarray, vertexbuffer, indexbuffer, indexcount);
}

void lre::RenderGrid(Plane plane, int count, int grid_size, const glm::vec4& color) {
	auto* vertexarray = Vertex::P1::VertexArray();

	
}

void lre::Destroy() {
	for (auto& pair: self->shader_registry) {
		delete pair.second;
	}

	Vertex::FreeVertexArray();
}


void lre::Test() {
	lre::Vertex::P1N1 test_vertices[] = {
		{{ -1, -0.5, -0.5 }, { 0.0, 0.0, 1.0 }},
		{{ -0.5, +0.5, -0.5 }, { 0.0, 0.0, 1.0 }},
		{{ +0.5, +0.5, -0.5 }, { 0.0, 0.0, 1.0 }},
		{{ +0.5, -0.5, -0.5 }, { 0.0, 0.0, 1.0 }},
	};

	uint32_t test_indices[] = {
		0 + 4 * 0, 1 + 4 * 0, 2 + 4 * 0, 2 + 4 * 0, 3 + 4 * 0, 0 + 4 * 0,
	};

	static lgl::VertexArray* vertexarray = lre::Vertex::P1N1::VertexArray();
	static lgl::VertexBuffer* vertexbuffer = nullptr;
	static lgl::IndexBuffer* indexbuffer = nullptr;

	if (vertexbuffer == nullptr) {
		vertexbuffer = new lgl::VertexBuffer();

		vertexbuffer->Bind();
		vertexbuffer->Allocate(sizeof(test_vertices));
		vertexbuffer->AddDataDynamic(test_vertices, sizeof(test_vertices));
	}

	if (indexbuffer == nullptr) {
		indexbuffer = new lgl::IndexBuffer();

		indexbuffer->Bind();
		indexbuffer->Allocate(sizeof(test_indices));
		indexbuffer->AddData(test_indices, sizeof(test_indices));
	}

	auto* shader = lre::GetShader("normal");
	shader->Bind();

	vertexarray->Bind();
	vertexarray->BindVertexBuffer(vertexbuffer, vertexarray->stride);
	vertexarray->BindIndexBuffer(indexbuffer);

	lgl::DrawIndexed(lgl::TRIANGLE, 6, lgl::UNSIGNED_INT, nullptr);
}