#pragma once

#include "helper.hpp"
#include <cstdlib>
#include <cmath>


template <class T>
int Helper::sgn(T v) {
    return (v > T(0)) - (v < T(0));
}

glm::vec3 Helper::sgn(glm::vec3& vec) {
    return glm::vec3(sgn(vec.x), sgn(vec.y), sgn(vec.z));
}

float Helper::rand_float() {
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

float Helper::rand_float(float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

glm::vec3 Helper::rand_cube(float size) {
	float x = rand_float(-size / 2, size / 2);
	float y = rand_float(-size / 2, size / 2);
	float z = rand_float(-size / 2, size / 2);
	return glm::vec3(x, y, z);
}

glm::vec3 Helper::rand_sphere() {
	// algo from: https://gist.github.com/andrewbolster/10274979
	float phi = rand_float(0, M_PI * 2);
	float costheta = rand_float(-1, 1);

	float theta = acosf(costheta);
	float x = sinf(theta) * cosf(phi);
	float y = sinf(theta) * sinf(phi);
	float z = cosf(theta);

	return glm::vec3(x, y, z);
}

glm::vec3 Helper::rand_sphere(float radius) {
	float mag = rand_float() * radius * radius;
	return rand_sphere() * std::sqrtf(mag);
}

float Helper::length_squared(const glm::vec3& vec) {
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

float Helper::length(const glm::vec3& vec) {
	return sqrtf(length_squared(vec));
}

glm::vec3 Helper::normalize(const glm::vec3& vec) {
	return vec / length(vec);
}

glm::vec3 Helper::round(const glm::vec3& vec) {
	return glm::vec3(roundf(vec.x), roundf(vec.y), roundf(vec.z));
}

glm::vec3 Helper::max(const glm::vec3& a, const glm::vec3& b) {
	return glm::vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

float Helper::limit(float val, float lmt) {
	return std::abs(val) > lmt ? sgn(val) * lmt : val;
}

glm::vec3 Helper::limit(const glm::vec3& vec, const glm::vec3& lmt) {
	return glm::vec3(limit(vec.x, lmt.x), limit(vec.y, lmt.y), limit(vec.z, lmt.z));
}
