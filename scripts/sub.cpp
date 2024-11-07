#include "sub.hpp"
#include "helper.hpp"
#include <cmath>

Sub::Sub(const Loader& loader, LightManager& light_manager, glm::vec3 position) :
		geometry(loader.sub_geometry),
		texture(loader.sub_texture),
		light_manager(light_manager) {
	this->position = position;
	light1 = light_manager.add_spot(position + light_offset1, glm::vec3(1), light_direction, light_cut_off);
	light2 = light_manager.add_spot(position + light_offset2, glm::vec3(1), light_direction, light_cut_off);
	velocity = glm::vec3(0);
	glCreateBuffers(1, &ubo_buffer);
	glNamedBufferStorage(ubo_buffer, sizeof(ObjectUBO), &ubo, GL_DYNAMIC_STORAGE_BIT);
}

Sub::~Sub() {
	glDeleteBuffers(1, &ubo_buffer);
}

// return 1 where vector component is 0, else 0
glm::vec3 zero_component(const glm::vec3& vec) {
	return glm::vec3(vec.x == 0, vec.y == 0, vec.z == 0);
}

void Sub::update(float delta_s) {
	glm::mat4 rot_mat4 = glm::rotate(rotation, up_vector);
	glm::mat3 rot_mat3 = (glm::mat3)rot_mat4;
	vertical_limiter = 0;

	// rotation
	angular_input = input_rotation * angular_acceleration_const;
	angular_drag = angular_velocity * angular_velocity * angular_drag_const * Helper::sgn(angular_velocity);
	angular_acceleration = angular_input - angular_drag;
	if (sas) {
		angular_sas = input_rotation == 0 ? Helper::limit(angular_acceleration - angular_velocity / delta_s, angular_acceleration_const) : 0;
		angular_acceleration += angular_sas;
	}

	angular_velocity += angular_acceleration * delta_s;
	rotation += angular_velocity * delta_s;
	if (rotation < 0)
		rotation += 2 * M_PI;
	else if (rotation > 2 * M_PI)
		rotation -= 2 * M_PI;

	// lcl
	bool depth_limit = position.y <= max_depth && velocity.y < 0 || position.y >= min_depth && velocity.y > 0;
	lcl_velocity = glm::transpose(rot_mat3) * velocity;
	lcl_input = (glm::vec3)input_move * acceleration_const;
	if (depth_limit)
		lcl_input.y = 0;
	lcl_drag = lcl_velocity * lcl_velocity * drag_const * Helper::sgn(lcl_velocity);
	lcl_acceleration = lcl_input - lcl_drag;
	if (sas) {
		lcl_sas = Helper::limit(lcl_acceleration - lcl_velocity / delta_s, acceleration_const) * zero_component(input_move);
		lcl_acceleration += lcl_sas;
	}
	if (depth_limit) {
		vertical_limiter = Helper::limit(lcl_acceleration.y - lcl_velocity.y / delta_s, acceleration_const.y);
		lcl_acceleration.y += vertical_limiter;
	}

	// glb
	acceleration = rot_mat3 * lcl_acceleration;
	
	// velocity, position
	velocity += acceleration * delta_s;	
	position += velocity * delta_s;

	// update model matrix
	ubo.model_matrix = glm::translate(position) * rot_mat4;
}

void Sub::draw() {
	glNamedBufferSubData(ubo_buffer, 0, sizeof(ubo.model_matrix), &ubo.model_matrix);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo_buffer);
	glBindTextureUnit(3, texture);
	geometry->draw();
}

void Sub::update_camera_ubo(CameraUBO *camera) {
	glm::vec3 camera_position = ubo.model_matrix * glm::vec4(camera_offset, 1);
	glm::vec3 look_at = ubo.model_matrix * glm::vec4(look_at_offset, 1);
	camera->position = glm::vec4(camera_position, 1);
	camera->view = glm::lookAt(camera_position, look_at, up_vector);
}

void Sub::update_lights_ubos() {
	LightUBO* l1 = light_manager.get_light(light1);
	LightUBO* l2 = light_manager.get_light(light2);
	l1->position = ubo.model_matrix * glm::vec4(light_offset1, 1);
	l2->position = ubo.model_matrix * glm::vec4(light_offset2, 1);

	glm::vec4 direction = glm::vec4((glm::mat3)glm::rotate(rotation, up_vector) * light_direction, 1);
	l1->direction = direction;
	l2->direction = direction;
}

void Sub::on_key_pressed(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS && key == GLFW_KEY_T) {
		sas = !sas;
		if (!sas) {
			lcl_sas = glm::vec3(0);
			angular_sas = 0;
		}
		return;
	}

	int val = action == GLFW_PRESS ? 1 : 0;
	
	if (action != GLFW_REPEAT) {
		switch (key)
		{
		case GLFW_KEY_W:
			input_move.z = -val;
			break;
		case GLFW_KEY_S:
			input_move.z = val;
			break;
		case GLFW_KEY_A:
			input_move.x = -val;
			break;
		case GLFW_KEY_D:
			input_move.x = val;
			break; 
		case GLFW_KEY_LEFT_SHIFT:
			input_move.y = val;
			break;
		case GLFW_KEY_LEFT_CONTROL:
			input_move.y = -val;
			break;
		case GLFW_KEY_Q:
			input_rotation = val;
			break; 
		case GLFW_KEY_E:
			input_rotation = -val;
			break;
		default:
			break;
		}
	}
}
