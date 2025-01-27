#pragma once

#include <cstdlib>
#include <random>

namespace art {

	// Generate random number in range [0, 1)
	inline float Random() {
		static thread_local std::mt19937 generator(std::random_device{}());
		std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
		return distribution(generator);
	}

	inline float Random(float min, float max) {
		return min + (max - min) * Random();
	}

	// Returns random point in the [-0.5, -0.5]-[0.5, 0.5] unit square.
	inline glm::vec2 RandomInSquare() {
		return glm::vec2(art::Random() - 0.5, art::Random() - 0.5);
	}

	// Random value from normal distribution (range is infinite)
	float RandomNormalDistribution() {
		float theta = 2 * art::pi * Random();
		float rho = std::sqrt(-2 * std::log(Random()));
		return rho * std::cos(theta);
	}

	// Returns vector to a random point in the stratified region of unit square
	glm::vec2 RandomInStratifiedSquare(uint32_t s_i, uint32_t s_j, float regionSize) {
		return glm::vec2(
			((s_i + Random()) * regionSize) - 0.5,
			((s_j + Random()) * regionSize) - 0.5
		);
	}

	// Get random unit vector
	// this funcion uses simple rejection sampling
	// i tried Gaussian distribution but it was slower
	glm::vec3 RandomVec() {
		while (true) {
			glm::vec3 v = glm::vec3(Random(-1, 1), Random(-1, 1), Random(-1, 1));
			float lensq = glm::dot(v, v);
			if (1e-160 < lensq && lensq <= 1) {
				return v / std::sqrt(lensq);
			}
		}
	}

	glm::vec3 RandomOnHemisphere(const glm::vec3 &N) {
		glm::vec3 v = RandomVec();
		if (glm::dot(v, N) > 0.0) {
			return v;
		} else {
			return -v;
		}
	}

	glm::vec3 RandomOnDisk() {
		while (true) {
			auto p = glm::vec3(Random(-1, 1), Random(-1, 1), 0);
			if (glm::dot(p, p) < 1)
				return p;
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