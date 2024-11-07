#include "floor.hpp"
#include "helper.hpp"
#include <cstdio>

Floor::Floor(const std::shared_ptr<Geometry> geometry, float geometry_size, float vertical_offset, GLuint texture, Material material, const Sub& sub) :
		geometry(geometry), 
		texture(texture),
		sub(sub),
		size(geometry_size),
		vertical_offset(vertical_offset) {
	ubo = {
		.model_matrix = glm::mat4(),
		.ambient_color = material.ambient_color,
		.diffuse_color = material.diffuse_color,
		.specular_color = material.specular_color,
		.has_texture = texture
	};

	glCreateBuffers(1, &ubo_buffer);
	glNamedBufferStorage(ubo_buffer, sizeof(ObjectUBO), &ubo, GL_DYNAMIC_STORAGE_BIT);
}

Floor::~Floor() {
	glDeleteBuffers(1, &ubo_buffer);
}

void Floor::draw_tile(const glm::vec3& position) {
	ubo.model_matrix = glm::translate(position);
	glNamedBufferSubData(ubo_buffer, 0, sizeof(ubo.model_matrix), &ubo.model_matrix);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo_buffer);
	glBindTextureUnit(3, texture);
	geometry->draw();
}

void Floor::draw() {
	grid_position = sub.position / size;
	rounded_grid_position = Helper::round(grid_position);
	position = glm::vec3(rounded_grid_position.x * size, vertical_offset, rounded_grid_position.z * size);
	
	draw_tile(position);
	draw_tile(position + glm::vec3(size, 0, 0));
	draw_tile(position + glm::vec3(0, 0, -size));
	draw_tile(position + glm::vec3(size, 0, -size));
}