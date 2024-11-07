#pragma once

#include "uniform_structs.hpp"
#include "loader.hpp"
#include "sub.hpp"

class Floor {
	float vertical_offset;
	int size;

	const Sub& sub;

	std::shared_ptr<Geometry> geometry;
	GLuint texture;
	GLuint ubo_buffer;
	ObjectUBO ubo;

public:
	glm::vec3 grid_position = glm::vec3(0);
	glm::vec3 rounded_grid_position = glm::vec3(0);
	glm::vec3 position = glm::vec3(0);

public:
	Floor(const std::shared_ptr<Geometry> geometry, float geometry_size, float vertical_offset, GLuint texture, Material material, const Sub& sub);
	~Floor();

	void draw();

private:
	void draw_tile(const glm::vec3& position);
};