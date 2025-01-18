#pragma once

#include "glm/glm.hpp"

namespace art {
	class Texture {
	public:
		virtual ~Texture() = default;

		virtual glm::vec3 Sample(float u, float v, const glm::vec3 &p) const = 0;
	};


	class SolidColorTexture : public Texture {
	public:
		SolidColorTexture(const glm::vec3 &albedo) : m_albedo(albedo) {}

		glm::vec3 Sample(float u, float v, const glm::vec3 &p) const override {
			return m_albedo;
		}

	private:
		glm::vec3 m_albedo;
	};


	class CheckerTexture : public Texture {
	public:
		CheckerTexture(float scale, const Texture *evenTexture, const Texture *oddTexture) :
			m_scale(scale),
			m_evenTexture(evenTexture),
			m_oddTexture(oddTexture) {}

		glm::vec3 Sample(float u, float v, const glm::vec3 &p) const override {
			int x = static_cast<int>(std::floor(m_scale * p.x));
			int y = static_cast<int>(std::floor(m_scale * p.y));
			int z = static_cast<int>(std::floor(m_scale * p.z));

			if ((x + y + z) % 2 == 0) {
				return m_evenTexture->Sample(u, v, p);
			} else {
				return m_oddTexture->Sample(u, v, p);
			}
		}

	private:
		float          m_scale;
		const Texture *m_evenTexture;
		const Texture *m_oddTexture;
	};


	class CheckerSphereTexture : public Texture {
	public:
		CheckerSphereTexture(float scale, const Texture *evenTexture, const Texture *oddTexture) :
			m_scale(scale),
			m_evenTexture(evenTexture),
			m_oddTexture(oddTexture) {
		}

		glm::vec3 Sample(float u, float v, const glm::vec3 &p) const override {
			int ui = static_cast<int>(std::floor(u * m_scale));
			int vi = static_cast<int>(std::floor(v * m_scale));

			if ((ui + vi) % 2 == 0) {
				return m_evenTexture->Sample(u, v, p);
			} else {
				return m_oddTexture->Sample(u, v, p);
			}
		}

	private:
		float          m_scale;
		const Texture *m_evenTexture;
		const Texture *m_oddTexture;
	};
}

