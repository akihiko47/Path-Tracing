#pragma once

#include <glm/glm.hpp>

namespace art {
	class Ray {
	public:
		Ray() : m_origin(glm::vec3(0)), m_direction(glm::vec3(0)) {}
		Ray(glm::vec3 origin, glm::vec3 direction) : m_origin(origin), m_direction(direction) {}

		glm::vec3 GetOrigin()    const { return m_origin; }
		glm::vec3 GetDirection() const { return m_direction; }

		glm::vec3 At(float t) const { return m_origin + t * m_direction; }

	private:
		glm::vec3 m_origin;
		glm::vec3 m_direction;
	};
}
