#include "flock.hpp"
#include "helper.hpp"

Flock::Flock(int fish_count, std::shared_ptr<Geometry> fish_geom, GLuint fish_texture) :
		visual_range_squared(visual_range * visual_range),
		fish(std::vector<TransformBase>(fish_count)),
		geometry(fish_geom),
		texture(fish_texture) {
	for (int i = 0; i < fish_count; ++i) {
		fish[i] = {
			.position = glm::vec3(
				Helper::rand_float(-max_sub_dist, max_sub_dist), 
				Helper::rand_float(max_depth, min_depth),
				Helper::rand_float(-max_sub_dist, max_sub_dist)),
			.velocity = Helper::rand_sphere(max_speed)
		};
	}

	glCreateBuffers(1, &material_buffer);
	glNamedBufferStorage(material_buffer, sizeof(Material), &material, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &fish_buffer);
	glNamedBufferStorage(fish_buffer, sizeof(TransformBase) * fish_count, fish.data(), GL_DYNAMIC_STORAGE_BIT);
}

Flock::~Flock() {
	glDeleteBuffers(1, &material_buffer);
	glDeleteBuffers(1, &fish_buffer);
}

void Flock::update(float delta, const Sub& sub) {
	for (TransformBase& f : fish) {
		glm::vec3 avoid_acc = glm::vec3();
		glm::vec3 avg_position = glm::vec3();
		glm::vec3 avg_velocity = glm::vec3();
		int neighbours = 0;

		// submarine interaction
		glm::vec3 sub_dir = sub.position - f.position;
		bool sub_interaction = false;
		float dist_squared = Helper::length_squared(sub_dir);
		if (dist_squared > max_sub_dist * max_sub_dist) {
			f.velocity += Helper::normalize(sub_dir) * sub_acc;
			sub_interaction = true;
		}
		if (dist_squared < min_sub_dist * min_sub_dist) {
			f.velocity -= Helper::normalize(sub_dir) * sub_acc;
			sub_interaction = true;
		}

		if (!sub_interaction) {
			for (const TransformBase& other : fish) {
				if (&f == &other) // same instance
					continue;

				glm::vec3 to_other = other.position - f.position;
				float dist_squared = Helper::length_squared(to_other);
				if (dist_squared < visual_range_squared) {
					++neighbours;
					avg_position += other.position;
					avg_velocity += other.velocity;
					glm::vec3 dir_to_other = other.position - f.position;
					avoid_acc += -glm::vec3(1 / dir_to_other.x, 1 / dir_to_other.y, 1 / dir_to_other.z);
				}
			}

			if (neighbours > 0) {
				avg_position /= neighbours;
				avg_velocity /= neighbours;

				// fly towards center
				f.velocity += (avg_position - f.position) * coherence;
				// avoid others
				f.velocity += avoid_acc * separation;
				// match velocity
				f.velocity += (avg_velocity - f.velocity) * alignment;
			}
		}

		// height limit
		if (f.velocity.y > 0) {
			float needed_acc = f.velocity.y * f.velocity.y / (min_depth - f.position.y);
			if (needed_acc > hight_acc_min)
				f.velocity.y -= Helper::limit(needed_acc, hight_acc_max);
		}

		else if (f.velocity.y < 0) {
			float needed_acc = f.velocity.y * f.velocity.y / (f.position.y - max_depth);
			if (needed_acc > hight_acc_min)
				f.velocity.y += Helper::limit(needed_acc, hight_acc_max);
		}
			

		// limit velocity
		float speed = Helper::length(f.velocity);
		if (speed > max_speed)
			f.velocity = Helper::normalize(f.velocity) * max_speed;

		// update position
		f.position += f.velocity * delta;
	}
}

void Flock::draw() {
	glNamedBufferSubData(fish_buffer, 0, fish.size() * sizeof(TransformBase), fish.data());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, fish_buffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, material_buffer);
	glBindTextureUnit(3, texture);
	geometry->draw_instanced(fish.size());
}
