#pragma once

#include "uniform_structs.hpp"
#include <vector>
#include "pv112_application.hpp"
#include <cstdlib>

class LightManager {
	bool buffer_created = false;
	GLuint lights_buffer;
	std::vector<LightUBO> lights = std::vector<LightUBO>();

public:
	~LightManager();

	void create_buffer();
	void bind_lights();

	unsigned int add_sun(glm::vec3 position, glm::vec3 color);
	unsigned int add_spot(glm::vec3 position, glm::vec3 color, glm::vec3 direction, float cut_off);

	LightUBO* get_light(unsigned int light);
};