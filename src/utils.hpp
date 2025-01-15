#pragma once

#include <cstdlib>


namespace art {
	inline float Random() {
		return std::rand() / (RAND_MAX + 1.0);
	}

	inline float Random(float min, float max) {
		return min + (max - min) * Random();
	}

	inline glm::vec2 RandomInSquare() {
		// Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
		return glm::vec2(art::Random() - 0.5, art::Random() - 0.5);
	}

	inline glm::vec3 RandomVec() {
		return glm::normalize(glm::vec3(Random() - 0.5, Random() - 0.5, Random() - 0.5));
	}

	glm::vec3 RandomOnHemisphere(const glm::vec3 &N) {
		glm::vec3 v = RandomVec();
		if (glm::dot(v, N) > 0.0) {
			return v;
		} else {
			return -v;
		}
	}

	bool VecNearZero(const glm::vec3& v) {
		float s = 1e-8;
		return (std::fabs(v.x) < s) && (std::fabs(v.y) < s) && (std::fabs(v.z) < s);
	}

	float SchlicksReflectance(float cos, float refractionIndex) {
		float r0 = (1 - refractionIndex) / (1 + refractionIndex);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow((1 - cos), 5);
	}
}