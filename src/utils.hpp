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
}