#pragma once

#include "hittable.hpp"
#include "utils.hpp"

namespace art {
	class Material {
	public:
		virtual ~Material() = default;

		virtual bool scatter(const Ray &rayIn, const HitInfo &hitInfo, glm::vec3 &attenuation, Ray &rayOut) const {
			return false;
		}

	};


	class Lambertian : public Material {
	public:
		Lambertian(const glm::vec3 &albedo) : m_albedo(albedo) {}

		bool scatter(const Ray &rayIn, const HitInfo &hitInfo, glm::vec3 &attenuation, Ray &rayOut) const override {
			glm::vec3 scatterDir = hitInfo.N + RandomVec();

			if (VecNearZero(scatterDir)) {
				scatterDir = hitInfo.N;
			}

			rayOut = Ray(hitInfo.p, scatterDir);
			attenuation = m_albedo;
			return true;
		}

	private:
		glm::vec3 m_albedo;
	};


	class Metal : public Material {
	public:
		Metal(const glm::vec3 &albedo = glm::vec3(1), const float smoothness = 0.5) : m_albedo(albedo), m_smoothness(smoothness < 1 ? smoothness : 1) {}

		bool scatter(const Ray &rayIn, const HitInfo &hitInfo, glm::vec3 &attenuation, Ray &rayOut) const override {
			glm::vec3 reflectDir = glm::reflect(rayIn.GetDirection(), hitInfo.N);
			reflectDir = glm::normalize(reflectDir) + ((1 - m_smoothness) * RandomVec());
			rayOut = Ray(hitInfo.p, reflectDir);
			attenuation = m_albedo;
			return (glm::dot(rayOut.GetDirection(), hitInfo.N) > 0);  // check if we are not reflecting inside object
		}

	private:
		glm::vec3 m_albedo;
		float m_smoothness;
	};
}
