#pragma once

#include <algorithm>

#include "hittable.hpp"
#include "utils.hpp"
#include "texture.hpp"

namespace art {
	class IMaterial {
	public:
		virtual ~IMaterial() = default;

		virtual bool Scatter(const Ray &rayIn, const HitInfo &hitInfo, glm::vec3 &attenuation, Ray &rayOut) const = 0;
	};


	class Lambertian : public IMaterial {
	public:
		Lambertian(const glm::vec3 &albedo, float smoothness, float specularProbability, const ITexture *normals, float normalsStrength) :
			m_albedo(albedo),
			m_textureAlbedo(nullptr),
			m_textureNormals(normals),
			m_normalsStrength(normalsStrength),
			m_smoothness(smoothness),
			m_specularProbability(specularProbability) {}

		Lambertian(const ITexture *albedo, float smoothness, float specularProbability, const ITexture *normals, float normalsStrength) :
			m_albedo(glm::vec3(1)),
			m_textureAlbedo(albedo),
			m_textureNormals(normals),
			m_normalsStrength(normalsStrength),
			m_smoothness(smoothness),
			m_specularProbability(specularProbability) {}

		bool Scatter(const Ray &rayIn, const HitInfo &hitInfo, glm::vec3 &attenuation, Ray &rayOut) const override {

			glm::vec3 N;
			if (m_textureNormals) {
				glm::vec3 tangentSpaceNormal = m_textureNormals->Sample(hitInfo.u, hitInfo.v, hitInfo.p, glm::vec3(0)) * 2.0f - 1.0f;
				tangentSpaceNormal.x *= m_normalsStrength;
				tangentSpaceNormal.y *= m_normalsStrength;
				tangentSpaceNormal = glm::normalize(tangentSpaceNormal);

				glm::mat3 TBN = glm::mat3(hitInfo.T, hitInfo.BT, hitInfo.N);
				N = glm::normalize(TBN * tangentSpaceNormal);
			} else {
				N = hitInfo.N;
			}
			 
			glm::vec3 diffuseDir = N + RandomVec();
			glm::vec3 reflectDir = glm::reflect(rayIn.GetDirection(), N);

			if (VecNearZero(diffuseDir)) {
				diffuseDir = N;
			}

			bool isSpecularBounce = m_specularProbability >= Random();

			glm::vec3 dir = glm::mix(diffuseDir, reflectDir, m_smoothness * isSpecularBounce);

			attenuation = glm::mix(
				m_textureAlbedo ? m_textureAlbedo->Sample(hitInfo.u, hitInfo.v, hitInfo.p, glm::vec3(0)) : m_albedo,
				glm::vec3(1),
				isSpecularBounce
			);

			rayOut = Ray(hitInfo.p, dir);

			return true;
		}

	private:
		glm::vec3       m_albedo;
		const ITexture *m_textureAlbedo;
		const ITexture *m_textureNormals;
		float           m_normalsStrength;
		float           m_smoothness;
		float           m_specularProbability;
	};


	class Metal : public IMaterial {
	public:
		Metal(const glm::vec3 &albedo, float smoothness) : 
			m_albedo(albedo),
			m_textureAlbedo(nullptr),
			m_smoothness(std::clamp(smoothness, 0.0f, 1.0f)) {}

		Metal(const ITexture *albedo, float smoothness) :
			m_albedo(glm::vec3(1)),
			m_textureAlbedo(albedo),
			m_smoothness(std::clamp(smoothness, 0.0f, 1.0f)) {
		}

		bool Scatter(const Ray &rayIn, const HitInfo &hitInfo, glm::vec3 &attenuation, Ray &rayOut) const override {
			glm::vec3 reflectDir = glm::reflect(rayIn.GetDirection(), hitInfo.N);
			reflectDir = glm::normalize(glm::normalize(reflectDir) + ((1 - m_smoothness) * RandomVec()));
			rayOut = Ray(hitInfo.p, reflectDir);

			attenuation = m_textureAlbedo ? m_textureAlbedo->Sample(hitInfo.u, hitInfo.v, hitInfo.p, glm::vec3(0)) : m_albedo;

			return (glm::dot(rayOut.GetDirection(), hitInfo.N) > 0);  // check if we are not reflecting inside object
		}

	private:
		glm::vec3       m_albedo;
		const ITexture *m_textureAlbedo;
		float           m_smoothness;
	};


	class Dielectric : public IMaterial {
	public:
		Dielectric(float refractionIndex, const glm::vec3 &albedo, float smoothness) :
			m_refractionIndex(refractionIndex),
			m_albedo(albedo),
			m_smoothness(std::clamp(smoothness, 0.0f, 1.0f)) {}

		bool Scatter(const Ray &rayIn, const HitInfo &hitInfo, glm::vec3 &attenuation, Ray &rayOut) const override {
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

	class DiffuseLight : public IMaterial {
	public:
		DiffuseLight(const glm::vec3 &albedo) :
			m_albedo(albedo),
			m_textureAlbedo(nullptr) {}

		DiffuseLight(const ITexture *albedo) :
			m_albedo(glm::vec3(0)),
			m_textureAlbedo(albedo) {
		}

		bool Scatter(const Ray &rayIn, const HitInfo &hitInfo, glm::vec3 &attenuation, Ray &rayOut) const override {
			attenuation = m_textureAlbedo ? m_textureAlbedo->Sample(hitInfo.u, hitInfo.v, hitInfo.p, glm::vec3(0)) : m_albedo;
			return false;
		}

	private:
		glm::vec3       m_albedo;
		const ITexture *m_textureAlbedo;
	};
}
