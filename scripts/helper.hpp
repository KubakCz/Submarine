#pragma once

#include "pv112_application.hpp"

static class Helper {
public:
	// signum
	template <class T>
	static int sgn(T v);

	// aply signum componentvise
	static glm::vec3 sgn(glm::vec3& vec);

	static float limit(float val, float limit);

	// random
	static float rand_float();

	static float rand_float(float min, float max);

	static glm::vec3 rand_cube(float size);

	static glm::vec3 rand_sphere();
	
	static glm::vec3 rand_sphere(float radius);

	// vector operations
	static float length_squared(const glm::vec3& vec);

	static float length(const glm::vec3& vec);

	static glm::vec3 normalize(const glm::vec3& vec);

	static glm::vec3 round(const glm::vec3& vec);

	static glm::vec3 max(const glm::vec3& a, const glm::vec3&);

	static glm::vec3 limit(const glm::vec3& vec, const glm::vec3& limit);
};