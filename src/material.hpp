#pragma once

#include "hittable.hpp"
#include "utils.hpp"
#include "texture.hpp"

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
		Lambertian(const Texture *albedo) : m_albedo(albedo) {}

		bool scatter(const Ray &rayIn, const HitInfo &hitInfo, glm::vec3 &attenuation, Ray &rayOut) const override {
			glm::vec3 scatterDir = glm::normalize(hitInfo.N + RandomVec());

			if (VecNearZero(scatterDir)) {
				scatterDir = hitInfo.N;
			}

			rayOut = Ray(hitInfo.p, scatterDir);
			attenuation = m_albedo->Sample(hitInfo.u, hitInfo.v, hitInfo.p);
			return true;
		}

	private:
		const Texture *m_albedo;
	};


	class Metal : public Material {
	public:
		Metal(const glm::vec3 &albedo = glm::vec3(1), float smoothness = 0.5) : m_albedo(albedo), m_smoothness(smoothness < 1 ? smoothness : 1) {}

		bool scatter(const Ray &rayIn, const HitInfo &hitInfo, glm::vec3 &attenuation, Ray &rayOut) const override {
			glm::vec3 reflectDir = glm::reflect(rayIn.GetDirection(), hitInfo.N);
			reflectDir = glm::normalize(glm::normalize(reflectDir) + ((1 - m_smoothness) * RandomVec()));
			rayOut = Ray(hitInfo.p, reflectDir);
			attenuation = m_albedo;
			return (glm::dot(rayOut.GetDirection(), hitInfo.N) > 0);  // check if we are not reflecting inside object
		}

	private:
		glm::vec3 m_albedo;
		float m_smoothness;
	};


	class Dielectric : public Material {
	public:
		Dielectric(float refractionIndex, const glm::vec3 &albedo = glm::vec3(1), float smoothness = 0.5) :
			m_refractionIndex(refractionIndex),
			m_albedo(albedo),
			m_smoothness(smoothness < 1 ? smoothness : 1) {}

		bool scatter(const Ray &rayIn, const HitInfo &hitInfo, glm::vec3 &attenuation, Ray &rayOut) const override {
			float ri = hitInfo.frontFace ? (1.0 / m_refractionIndex) : m_refractionIndex;

			float cosTh = std::fmin(glm::dot(-rayIn.GetDirection(), hitInfo.N), 1.0);
			float sinTh = std::sqrt(1.0 - cosTh * cosTh);
			bool cantRefract = ri * sinTh > 1.0;

			glm::vec3 offset = (1 - m_smoothness) * RandomVec();
			glm::vec3 dir;
			if (cantRefract || art::SchlicksReflectance(cosTh, ri) > Random()) {
				dir = glm::reflect(rayIn.GetDirection(), hitInfo.N) + offset;
				attenuation = glm::vec3(1);
			} else {
				dir = glm::refract(rayIn.GetDirection(), hitInfo.N, ri) + offset;
				attenuation = m_albedo;
			}

			if (art::VecNearZero(dir)) {
				return false;
			}

			rayOut = Ray(hitInfo.p, dir);
			return true;
		}

	private:

		float     m_refractionIndex;  // in air or ratio of enclosing media (if not inside air)
		glm::vec3 m_albedo;
		float     m_smoothness;
	};
}
