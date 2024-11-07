#include "light_manager.hpp"
#include <stdexcept>

LightManager::~LightManager() {
	if (buffer_created)
		glDeleteBuffers(1, &lights_buffer);
}

void LightManager::create_buffer() {
	if (buffer_created)
		throw std::domain_error("Buffer already created.");

	glCreateBuffers(1, &lights_buffer);

	glNamedBufferStorage(lights_buffer, sizeof(LightUBO) * lights.size(), lights.data(), GL_DYNAMIC_STORAGE_BIT);
	buffer_created = true;
}

void LightManager::bind_lights() {
	if (!buffer_created)
		throw std::domain_error("Buffer not created.");

	glNamedBufferSubData(lights_buffer, 0, sizeof(LightUBO) * lights.size(), lights.data());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lights_buffer);
}

unsigned int LightManager::add_sun(glm::vec3 position, glm::vec3 color) {
	if (buffer_created)
		throw std::domain_error("Buffer already created. Can not add any other lights.");

	LightUBO light = {
		.position = glm::vec4(position, 0),
		.color = glm::vec4(color, 0.0f),
		.direction = glm::vec4(0),
		.cut_off = 0
	};
	lights.push_back(light);
	return lights.size() - 1;
}

unsigned int LightManager::add_spot(glm::vec3 position, glm::vec3 color, glm::vec3 direction, float cut_off) {
	if (buffer_created)
		throw std::domain_error("Buffer already created. Can not add any other lights.");

	LightUBO light = {
		.position = glm::vec4(position, 1),
		.color = glm::vec4(color, 0.0f),
		.direction = glm::vec4(direction, 1),
		.cut_off = std::cosf(cut_off)
	};
	lights.push_back(light);
	return lights.size() - 1;
}

LightUBO* LightManager::get_light(unsigned int light) {
	return &lights[light];
}
